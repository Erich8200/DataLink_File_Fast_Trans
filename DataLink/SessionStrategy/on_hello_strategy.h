#pragma once

#include "AsyncServer_VS/Session/session_strategy.h"

class onHelloStrategy : public SessionStrategy
{
public:
    onHelloStrategy();
    void run(std::weak_ptr<Session> session, std::shared_ptr<char []> payload_packet_data) override;

private:
    bool                   m_challenged;
    CryptoPP::word32       m_rand_num;
//    bool                   m_is_valid_session;
};
