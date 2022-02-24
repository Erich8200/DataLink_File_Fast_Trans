#ifndef MY_GLOBAL_H
#define MY_GLOBAL_H

#include <QStandardItemModel>
#include <QString>
#include <atomic>
#include <map>
#include <mutex>

class SessionManager; // 前置声明

// 任务模型
extern QStandardItemModel*                              g_task_list_model;
extern std::mutex                                       g_task_list_mutex;
// 当前处理完的任务数量
extern std::atomic<size_t>                              g_processed_task_count;
// 传输任务ID
extern size_t                                           g_task_id;
 // 每个Session是否通过了挑战
extern std::map<unsigned long long, std::atomic<bool> > g_session_validation_states;
extern std::mutex                                       g_session_validation_states_mutex;

extern std::shared_ptr<SessionManager>                  g_session_manager;

namespace PacketType {
    enum { HELLO, FILE };
}

namespace TaskTableColumn {
    enum { FILE_NAME, FILE_SIZE, STATE, ID, ITEM_COUNT };
}

namespace FileTransState {
    const QString waiting     = "就绪";
    const QString sending     = "传输中";
    const QString finished    = "传输完成";
    const QString open_failed = "打开失败";
}

int getRowFromModelById (QStandardItemModel* model, unsigned long long id, int col);

#endif // MY_GLOBAL_H
