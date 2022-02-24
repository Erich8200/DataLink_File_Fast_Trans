#ifndef MAINWIDGET_H
#define MAINWIDGET_H

// Qt
#include <QWidget>
#include <QStandardItemModel>
#include <QString>
#include <QDateTime>
#include <QFileDialog>

// std
#include <memory>

// AsyncServer
#include "AsyncServer_VS/Session/session_manager.h"
#include "AsyncServer_VS/Communication/Server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class EventType {
 public:
    QString connect = QString("新连接");
    QString disconnect = QString("断开连接");
};

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void on_btnSelectDir_clicked();

    void on_btnOpenDir_clicked();

private:
    Ui::MainWidget *ui;
    std::shared_ptr<SessionManager> m_session_manager;
    std::shared_ptr<Server>         m_server;
    bool                            m_server_started;

    EventType eventType;

    void on_new_conn(unsigned long long index, std::string ip);
    void on_del_conn(unsigned long long index);

    void resizeEvent(QResizeEvent *event) override;
};


#endif // MAINWIDGET_H
