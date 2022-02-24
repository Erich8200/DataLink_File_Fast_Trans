#include "my_global.h"

QStandardItemModel*                  g_conn_model;
QStandardItemModel*                  g_file_model;
std::mutex                           g_conn_model_mutex;
std::mutex                           g_file_model_mutex;

QString                              g_file_store_dir;

std::atomic<unsigned long long>      g_file_trans_task_ID;

int getRowFromModelById (QStandardItemModel* model, unsigned long long id, int col) {
    auto items = model->findItems(QString::number(id), Qt::MatchExactly, col);
    int row = -1;
    if (!items.empty()) {
        row = items.first()->index().row();
    }
    return row;
}

bool checkSessionValidation(unsigned long long id)
{
    int row = getRowFromModelById(g_conn_model, id, ConnTableColumn::ID);
    return g_conn_model->item(row, ConnTableColumn::Valid)->data().toBool();
}
