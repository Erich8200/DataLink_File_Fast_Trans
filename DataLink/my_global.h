#pragma once

#define MAX_PACKET_SIZE 128*1024
#define BUF_LEN 128*1024
#define FR_BUF_LEN 127*1024
#define PACKET_MAGIC 0x82006666

#include <QStandardItemModel>
#include <mutex>
#include <string>
#include <atomic>

namespace PacketType {
    enum {HELLO, FILE};
}

extern QStandardItemModel*                  g_conn_model;
extern QStandardItemModel*                  g_file_model;
extern std::mutex                           g_conn_model_mutex;
extern std::mutex                           g_file_model_mutex;

extern QString                              g_file_store_dir;

extern std::atomic<unsigned long long>      g_file_trans_task_ID;

int getRowFromModelById (QStandardItemModel* model, unsigned long long id, int col);
bool checkSessionValidation(unsigned long long id); // 返回Session合法性，即是否已经通过挑战

namespace FileTableColumn {
    enum { Name, Size, State, Checked, ID, ITEM_COUNT };
}

namespace ConnTableColumn {
    enum  { Time, Event, IP, ID, Valid, ITEM_COUNT };
}

namespace FileReceiveState {
    const static std::string RECEIVING = "接收中";
    const static std::string FINISHED  = "传输完成";
}

namespace FileCheckState {
    const static std::string UNCHECKED = "等待中";
    const static std::string CHECKING  = "计算中";
    const static std::string CHECHED  = "校验成功";
    const static std::string FAILED  = "校验失败";
}
