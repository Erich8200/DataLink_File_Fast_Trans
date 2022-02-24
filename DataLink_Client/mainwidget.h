#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "AsyncClientLib/Communication/client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void on_btnSelectFile_clicked();

    void on_btnClearList_clicked();

    void on_btnStartTrans_clicked();

    void sendFile();

    void block_buttons();

    void recover_buttons();

private:
    Ui::MainWidget                          *ui;
    QString                                 m_ip;
    QString                                 m_port;
    size_t                                  m_thread_count;

    // 单客户端多Session连接实现多线程传输
    std::shared_ptr<Client>                 m_client;

    void resizeEvent(QResizeEvent *event) override;

signals:
    void start_block_buttons();
    void stop_block_buttons();

};
#endif // MAINWIDGET_H
