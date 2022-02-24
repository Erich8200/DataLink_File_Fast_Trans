#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "my_global.h"
#include "StrategyFactory/on_hello_strategy_factory.h"
#include "Packet/hello_packet.h"
#include "Packet/file_data_packet.h"
#include "Packet/file_info_packet.h"

#include <QRegExp>
#include <QFileDialog>
#include <QMessageBox>
#include <thread>
#include <chrono>
#include <QFileInfo>
#include <QFile>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
//    ui->ledtIP->setInputMask("000.000.000.000");
    g_task_list_model = new QStandardItemModel(this);

    QStringList headers;
    headers.append("文件名");
    headers.append("文件大小");
    headers.append("传输状态");
    headers.append("任务ID");
    g_task_list_model->setHorizontalHeaderLabels(headers);
    ui->tabTask->setModel(g_task_list_model);
    ui->tabTask->setColumnHidden(TaskTableColumn::ID, true);

    // 实例化全局的SessionManager
    g_session_manager = std::make_shared<SessionManager>();
    // 向SessionManager添加Factory
    std::shared_ptr<StrategyFactory> on_hello_factory =
            std::make_shared<onHelloStrategyFactory>();
    g_session_manager->addStrategyFactory(on_hello_factory);

    g_task_id = g_processed_task_count = 0;

    connect(this, SIGNAL(start_block_buttons()), this, SLOT(block_buttons()));
    connect(this, SIGNAL(stop_block_buttons()), this, SLOT(recover_buttons()));

}

MainWidget::~MainWidget()
{
    delete ui;
}


void MainWidget::on_btnSelectFile_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "选择要传输的文件", "./", "");
    for (auto it = fileNames.begin(); it != fileNames.end(); ++it) {

        QFileInfo file_info(*it);
        if (file_info.isFile()) {
            QList<QStandardItem*> items;
            for (size_t i = 0; i < TaskTableColumn::ITEM_COUNT; i++)
                items.append(new QStandardItem());
            items.at(TaskTableColumn::FILE_NAME)->setData(*it);
            items.at(TaskTableColumn::FILE_NAME)->setText(*it);

            qint64 file_size = file_info.size();
            items.at(TaskTableColumn::FILE_SIZE)->setData(file_size);
            items.at(TaskTableColumn::FILE_SIZE)->setText(QString::number(file_size));

            items.at(TaskTableColumn::STATE)->setData(FileTransState::waiting);
            items.at(TaskTableColumn::STATE)->setText(FileTransState::waiting);

            items.at(TaskTableColumn::ID)->setData(g_task_id);
            items.at(TaskTableColumn::ID)->setText(QString::number(g_task_id));

            g_task_id += 1;

            g_task_list_model->appendRow(items);
        }
    }

    ui->tabTask->setColumnHidden(TaskTableColumn::ID, true);

}


void MainWidget::on_btnClearList_clicked()
{
    g_task_list_model->clear();
}


void MainWidget::on_btnStartTrans_clicked()
{
    m_ip = ui->ledtIP->text();
    QRegExp rx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
      if( !rx.exactMatch(m_ip) ) {
          QMessageBox::information(this, "错误", "ip地址格式错误，请重新输入");
          return;
    }
    m_port = ui->spinPort->text();
    m_thread_count = ui->spinThreadCount->value();

    // 实例化Client并启动其守护线程
    m_client = std::make_shared<Client>(g_session_manager);
    std::thread daemon_thread([this](){ m_client->startDaemon(); });
    daemon_thread.detach();

    // 多Session线程连接实现并行传输
    for (size_t i = 0; i < m_thread_count; i++) {
        std::thread send_thread(std::bind(&MainWidget::sendFile, this));
        send_thread.detach();
    }

}

void MainWidget::sendFile()
{
    emit start_block_buttons();
    unsigned long long session_id = m_client->startNewConnection(m_ip.toStdString(), m_port.toStdString());
    if (session_id < MAX_CONN_COUNT) {
        // 发送挑战请求包
        std::string hello_packet_str = HelloPacket::makeHelloPacket(CryptoUtils::get_rand_word32());
        std::string hello_packet_full_str = Packet::makePacket(hello_packet_str, PacketType::HELLO);
        std::shared_ptr<std::string> hello_packet_full_str_ptr = std::make_shared<std::string>(hello_packet_full_str);
        g_session_manager->SessionSend(session_id, hello_packet_full_str_ptr);

        // 自旋等待直到该Session通过挑战，在Strategy中答复挑战
        bool validated = false;
        do {
            {
                std::lock_guard<std::mutex> g(g_session_validation_states_mutex);
                validated = g_session_validation_states[session_id].load(std::memory_order_acquire);
            }
            if (validated)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } while (!validated);

        // 从g_taskListModel中获取任务数
        while (g_processed_task_count < g_task_list_model->rowCount()) {
            size_t task_line_num = g_processed_task_count.fetch_add(1, std::memory_order_acq_rel);
            QString file_name_qstr =
            g_task_list_model->item(task_line_num, TaskTableColumn::FILE_NAME)->data().toString();

            std::string file_path = file_name_qstr.toLocal8Bit().constData();

            std::ifstream fin(file_path, std::ios::in | std::ios::binary);
            if (fin) {

                 g_task_list_model->item(task_line_num, TaskTableColumn::STATE)->setData(
                             FileTransState::sending);
                 g_task_list_model->item(task_line_num, TaskTableColumn::STATE)->setText(
                             FileTransState::sending);

                // 发送文件信息包
                std::shared_ptr<std::string> packet_str = nullptr;
                QFileInfo file_info(file_name_qstr);
                int file_size = file_info.size();

                std::string filename_str = file_info.fileName().toUtf8().toStdString();  // 文件名转换成Utf编码,std::string包装
                packet_str = FileInfoPacket::makePacket(filename_str,
                                                        file_size, CryptoUtils::calcFileSHA256Hash(file_path));
                packet_str = Packet::makePacket(packet_str, PacketType::FILE);
                g_session_manager->SessionSend(session_id, packet_str);

                // 发送文件数据包
                std::shared_ptr<unsigned char[]> file_read_buf(new unsigned char[FR_BUF_LEN]);
                int readedBytes = 0;
                bool is_eof;
                do {
                    is_eof = fin.read(reinterpret_cast<char*>(file_read_buf.get()), FR_BUF_LEN).eof();
                    readedBytes = fin.gcount();

                    // 设置是否为最后一个文件数据包
                    if (readedBytes < FR_BUF_LEN) { // Maybe has a problem
                        packet_str = FileDataPacket::makePacket(file_read_buf.get(), readedBytes, true);
                    }
                    else {
                        packet_str = FileDataPacket::makePacket(file_read_buf.get(), readedBytes, false);
                    }

                    packet_str = Packet::makePacket(packet_str, PacketType::FILE);
                    g_session_manager->SessionSend(session_id, packet_str);

                } while (!is_eof);
                fin.close();

                g_task_list_model->item(task_line_num, TaskTableColumn::STATE)->setData(
                            FileTransState::finished);
                g_task_list_model->item(task_line_num, TaskTableColumn::STATE)->setText(
                            FileTransState::finished);
            }
            else {
                g_task_list_model->item(task_line_num, TaskTableColumn::STATE)->setData(
                            FileTransState::open_failed);
                g_task_list_model->item(task_line_num, TaskTableColumn::STATE)->setText(
                            FileTransState::open_failed);
                std::cerr << "File open failed" << std::endl;
            }

        }
    }

    // 传输完毕，关闭Session，重新启用控件
//    g_session_manager->SessionClose(session_id);
    emit stop_block_buttons();

}

void MainWidget::block_buttons()
{
    ui->btnSelectFile->setEnabled(false);
    ui->btnCancelFile->setEnabled(false);
    ui->btnClearList->setEnabled(false);
    ui->btnStartTrans->setEnabled(false);
}

void MainWidget::recover_buttons()
{
    ui->btnSelectFile->setEnabled(true);
    ui->btnCancelFile->setEnabled(true);
    ui->btnClearList->setEnabled(true);
    ui->btnStartTrans->setEnabled(true);
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    double width = ui->tabTask->width();
    ui->tabTask->setColumnWidth(TaskTableColumn::FILE_NAME, width/(TaskTableColumn::ITEM_COUNT - 1));
    ui->tabTask->setColumnWidth(TaskTableColumn::FILE_SIZE, width/(TaskTableColumn::ITEM_COUNT - 1));
    ui->tabTask->setColumnWidth(TaskTableColumn::STATE, width/(TaskTableColumn::ITEM_COUNT - 1));
//    ui->tabTask->setColumnWidth(TaskTableColumn::ID, width/TaskTableColumn::ITEM_COUNT);
}
