#include "on_file_recv_strategy.h"
#include "../Packet/file_packet.h"
#include "my_global.h"
#include "../Packet/file_info_packet.h"
#include "../Packet/file_data_packet.h"
#include "AsyncServer_VS/CryptoUtils.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

onFileRecvStrategy::onFileRecvStrategy()
{

}

void onFileRecvStrategy::run(std::weak_ptr<Session> session, std::shared_ptr<char[]> payload_packet_data)
{
    // 检查会话合法性
    bool check_flag = false;
    {
        std::lock_guard<std::mutex> g(g_conn_model_mutex);
        check_flag = checkSessionValidation(session.lock()->getID());
    }
    if (!check_flag) {
        session.lock()->setShouldClose(true);
        return;
    }

    std::shared_ptr<FilePacket> packet = std::reinterpret_pointer_cast<FilePacket>(payload_packet_data);

    if (packet->getType() == FilePacket::INFO) {
        //  文件信息接收
        std::shared_ptr<FileInfoPacket> file_info_packet = std::reinterpret_pointer_cast<FileInfoPacket>(packet);
        unsigned long long task_ID = g_file_trans_task_ID.fetch_add(1, std::memory_order_acq_rel);
        m_task_id = task_ID;                                                    // 任务ID
        m_file_checksum = file_info_packet->getFileCheckSum();                  // 校验和
        m_file_store_dir = g_file_store_dir.toLocal8Bit().constData();          // 文件存储位置

        QString filename_qstr = QString::fromUtf8(file_info_packet->getFileName().c_str(),
                                                  file_info_packet->getFileName().length()); // 从utf-8解码文件名
        std::string filename_str = std::string(filename_qstr.toLocal8Bit().constData(),
                                               filename_qstr.toLocal8Bit().length());

        m_file_name = m_file_store_dir + "/" + filename_str;                    // 完整文件路径
        m_file_size = file_info_packet->getFileSize();                          // 文件大小
        if (m_file_out_stream == nullptr) {
            m_file_out_stream = std::shared_ptr<std::ofstream>
                (new std::ofstream(m_file_name, std::ios::out | std::ios::binary));
        }

        // 在模型中添加文件信息
        QList<QStandardItem*> items;
        for (size_t i = 0; i < FileTableColumn::ITEM_COUNT; i++)
            items.append(new QStandardItem());
        items.at(FileTableColumn::Name)->setData(QString::fromLocal8Bit(m_file_name.c_str()));
        items.at(FileTableColumn::Name)->setText(QString::fromLocal8Bit(m_file_name.c_str()));
        items.at(FileTableColumn::Size)->setData(QString::number(m_file_size));
        items.at(FileTableColumn::Size)->setText(QString::number(m_file_size));
        items.at(FileTableColumn::State)->setData(QString::fromStdString(FileReceiveState::RECEIVING));
        items.at(FileTableColumn::State)->setText(QString::fromStdString(FileReceiveState::RECEIVING));
        items.at(FileTableColumn::Checked)->setData(false);
        items.at(FileTableColumn::Checked)->setText(QString::fromStdString(FileCheckState::UNCHECKED));
        items.at(FileTableColumn::ID)->setData(m_task_id);
        items.at(FileTableColumn::ID)->setText(QString::number(m_task_id));

        // 向模型中添加行
        {
            std::lock_guard<std::mutex> g(g_file_model_mutex);
            g_file_model->appendRow(items);
        }

    } else if (packet->getType() == FilePacket::DATA) {
        // 文件数据块接收
        std::shared_ptr<FileDataPacket> file_data_packet = std::reinterpret_pointer_cast<FileDataPacket>(packet);
        m_file_out_stream->write(reinterpret_cast<const char*>(file_data_packet->getData()), file_data_packet->getDataLen());
        if (file_data_packet->getIsLastPiece()) { // 判断是否为最后一块?
            m_file_out_stream->flush();
            m_file_out_stream->close();
            m_file_out_stream = nullptr;

            // 在模型中设置“传输完成”
            {
                std::lock_guard<std::mutex> g(g_file_model_mutex);
                int row = getRowFromModelById(g_file_model, m_task_id, FileTableColumn::ID);
                QStandardItem* item = new QStandardItem(QString::fromStdString(FileReceiveState::FINISHED));
                item->setData(QString::fromStdString(FileReceiveState::FINISHED));
                g_file_model->setItem(row, FileTableColumn::State, item);
            }

            // 开一个独立线程校验文件
            std::thread validate_thread(std::bind(&onFileRecvStrategy::validate_file_hash,
                                                  this, m_file_name, m_task_id, m_file_checksum));
            validate_thread.detach();
        }
    }
}

void onFileRecvStrategy::setFileStoreDir(QString dir)
{
    m_file_store_dir = dir.toLocal8Bit().constData();
}

void onFileRecvStrategy::validate_file_hash(std::string file_name, unsigned long long task_id, std::string target_hash)
{
    QFileInfo new_file_info(QString::fromLocal8Bit(file_name.c_str()));
    if (new_file_info.exists()) {

        // 根据校验和校验文件
        {
            std::lock_guard<std::mutex> g(g_file_model_mutex);
            int row = getRowFromModelById(g_file_model, task_id, FileTableColumn::ID);
            QStandardItem* item = new QStandardItem(QString::fromStdString(FileCheckState::CHECKING));
            item->setData(QString::fromStdString(FileCheckState::CHECKING));
            g_file_model->setItem(row, FileTableColumn::Checked, item);
        }

        std::string cal_sha256 = CryptoUtils::calcFileSHA256Hash(file_name);
        if (cal_sha256 == target_hash) {
            // 校验成功，设置“校验成功”
            std::lock_guard<std::mutex> g(g_file_model_mutex);
            int row = getRowFromModelById(g_file_model, task_id, FileTableColumn::ID);
            QStandardItem* item = new QStandardItem(QString::fromStdString(FileCheckState::CHECHED));
            item->setData(QString::fromStdString(FileCheckState::CHECHED));
            g_file_model->setItem(row, FileTableColumn::Checked, item);
        } else {
            // 校验失败，设置“校验失败”
            std::lock_guard<std::mutex> g(g_file_model_mutex);
            int row = getRowFromModelById(g_file_model, task_id, FileTableColumn::ID);
            QStandardItem* item = new QStandardItem(QString::fromStdString(FileCheckState::FAILED));
            item->setData(QString::fromStdString(FileCheckState::FAILED));
            g_file_model->setItem(row, FileTableColumn::Checked, item);
        }
    }
}
