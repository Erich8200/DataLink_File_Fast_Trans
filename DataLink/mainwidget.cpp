#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "my_global.h"
#include "StrategyFactory/on_hello_strategy_factory.h"
#include "StrategyFactory/on_file_recv_strategy_factory.h"
#include <QDesktopServices>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowTitle("DalaLink");
//    setAttribute(Qt::WA_DeleteOnClose, true); // 加上关闭窗体时会出错

    g_conn_model = new QStandardItemModel(this);
    g_file_model = new QStandardItemModel(this);

    ui->tabViewConn->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中模式为选中行
    ui->tabViewConn->setEditTriggers(QAbstractItemView::NoEditTriggers);// 设置不可编辑
    ui->tabViewFile->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabViewFile->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList headers;
    headers.append("文件名");
    headers.append("文件大小");
    headers.append("传输状态");
    headers.append("校验结果");
    headers.append("任务ID");
    g_file_model->setHorizontalHeaderLabels(headers);
//    connect(g_file_model, SIGNAL(columnsInserted(const QModelIndex&, int, int)), this, SLOT(set_fileTab_col_hidden()));
    ui->tabViewFile->setModel(g_file_model);
    ui->tabViewFile->setColumnHidden(FileTableColumn::ID, true);

    QStringList headers1;
    headers1.append("时间");
    headers1.append("事件");
    headers1.append("客户端IP");
    headers1.append("会话ID");
    headers1.append("会话合法性");
    g_conn_model->setHorizontalHeaderLabels(headers1);
    ui->tabViewConn->setModel(g_conn_model);
    ui->tabViewConn->setColumnHidden(ConnTableColumn::Valid, true);

    m_server_started = false;
    g_file_store_dir = ".";

    // 创建Server和SessionManager
    m_session_manager = std::make_shared<SessionManager>();
    // 为SessionManager设置组件
    m_session_manager->setNewSessionCallback(
                std::bind(&MainWidget::on_new_conn, this, std::placeholders::_1, std::placeholders::_2));
    m_session_manager->setDelSessionCallback(
                std::bind(&MainWidget::on_del_conn, this, std::placeholders::_1));

    // 向SessionManager添加Factory
    std::shared_ptr<StrategyFactory> on_hello_factory =
            std::make_shared<onHelloStrategyFactory>();
    m_session_manager->addStrategyFactory(on_hello_factory);
    std::shared_ptr<StrategyFactory> on_file_recv_factory =
            std::make_shared<onFileRecvStrategyFactory>(g_file_store_dir);
    m_session_manager->addStrategyFactory(on_file_recv_factory);

    m_server = std::make_shared<Server>(m_session_manager, 6666);
    m_server->startServer();
    m_server->startDaemon();
    m_session_manager->startDaemon();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_del_conn(unsigned long long index)
{
    QDateTime curDateTime=QDateTime::currentDateTime();
    QList<QStandardItem*> items;
    for (size_t i = 0; i < ConnTableColumn::ITEM_COUNT; i++)
        items.append(new QStandardItem());
    items.at(ConnTableColumn::ID)->setData(index);
    items.at(ConnTableColumn::ID)->setText(QString::number(index));
    items.at(ConnTableColumn::IP)->setData(QString::fromStdString(""));
    items.at(ConnTableColumn::IP)->setText(QString::fromStdString(""));
    items.at(ConnTableColumn::Event)->setData(eventType.disconnect);
    items.at(ConnTableColumn::Event)->setText(eventType.disconnect);
    items.at(ConnTableColumn::Time)->setData(curDateTime.toString("yyyy-MM-dd hh:mm:ss.zzz"));
    items.at(ConnTableColumn::Time)->setText(curDateTime.toString("yyyy-MM-dd hh:mm:ss.zzz"));
    items.at(ConnTableColumn::Valid)->setData(false);
    items.at(ConnTableColumn::Valid)->setText("失效");
    g_conn_model->appendRow(items);
}

void MainWidget::on_new_conn(unsigned long long index, std::string ip)
{
    QDateTime curDateTime=QDateTime::currentDateTime();
    QList<QStandardItem*> items;
    for (size_t i = 0; i < ConnTableColumn::ITEM_COUNT; i++)
        items.append(new QStandardItem());
    items.at(ConnTableColumn::ID)->setData(index);
    items.at(ConnTableColumn::ID)->setText(QString::number(index));
    items.at(ConnTableColumn::Event)->setData(eventType.connect);
    items.at(ConnTableColumn::Event)->setText(eventType.connect);
    items.at(ConnTableColumn::IP)->setData(QString::fromStdString(ip));
    items.at(ConnTableColumn::IP)->setText(QString::fromStdString(ip));
    items.at(ConnTableColumn::Time)->setData(curDateTime.toString("yyyy-MM-dd hh:mm:ss.zzz"));
    items.at(ConnTableColumn::Time)->setText(curDateTime.toString("yyyy-MM-dd hh:mm:ss.zzz"));
    items.at(ConnTableColumn::Valid)->setData(false);
    items.at(ConnTableColumn::Valid)->setText("未校验");
    g_conn_model->appendRow(items);
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    double width = ui->tabViewFile->rect().width();
    ui->tabViewFile->setColumnWidth(FileTableColumn::Name, width/9*6);
    ui->tabViewFile->setColumnWidth(FileTableColumn::Size, width/9);
    ui->tabViewFile->setColumnWidth(FileTableColumn::State, width/9);
    ui->tabViewFile->setColumnWidth(FileTableColumn::Checked, width/9);

    width = ui->tabViewConn->width();
    ui->tabViewConn->setColumnWidth(ConnTableColumn::Time, width/4);
    ui->tabViewConn->setColumnWidth(ConnTableColumn::Event, width/4);
    ui->tabViewConn->setColumnWidth(ConnTableColumn::IP, width/4);
    ui->tabViewConn->setColumnWidth(ConnTableColumn::ID, width/4);
//    ui->tabViewConn->setColumnWidth(ConnTableColumn::Valid, width/ConnTableColumn::ITEM_COUNT);
}


void MainWidget::on_btnSelectDir_clicked()
{
    g_file_store_dir = QFileDialog::getExistingDirectory(this, "选择文件存放位置",
                                                    g_file_store_dir,
                                                    QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
    if (g_file_store_dir.isEmpty()) {
        g_file_store_dir = ".";
    }
    ui->labStorePathShow->setText(g_file_store_dir);
}

void MainWidget::on_btnOpenDir_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(g_file_store_dir + "/"));
}

