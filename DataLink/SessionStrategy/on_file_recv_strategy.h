#ifndef ON_FILE_RECV_STRATEGY_H
#define ON_FILE_RECV_STRATEGY_H

#include <QString>
#include <fstream>
#include <string>
#include <memory>
#include "AsyncServer_VS/Session/session_strategy.h"

class onFileRecvStrategy : public SessionStrategy
{
public:
    onFileRecvStrategy();
    void run(std::weak_ptr<Session> session, std::shared_ptr<char []> payload_packet_data) override;
    void setFileStoreDir(QString dir);

private:
    unsigned long long                    m_task_id;
    std::string                           m_file_store_dir;
    std::shared_ptr<std::ofstream>        m_file_out_stream;
    std::string                           m_file_name;
    int                                   m_file_size;
    std::string                           m_file_checksum;

    void validate_file_hash(std::string file_name, unsigned long long task_id, std::string target_hash);
};

#endif // ON_FILE_RECV_STRATEGY_H
