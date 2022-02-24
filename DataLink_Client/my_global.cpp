#include "my_global.h"

QStandardItemModel*                              g_task_list_model;
std::mutex                                       g_task_list_mutex;
std::atomic<size_t>                              g_processed_task_count;
size_t                                           g_task_id;
std::shared_ptr<SessionManager>                  g_session_manager;
std::map<unsigned long long, std::atomic<bool> > g_session_validation_states;
std::mutex                                       g_session_validation_states_mutex;

int getRowFromModelById (QStandardItemModel* model, unsigned long long id, int col) {
    auto items = model->findItems(QString::number(id), Qt::MatchExactly, col);
    int row = -1;
    if (!items.empty()) {
        row = items.first()->index().row();
    }
    return row;
}
