#pragma once

#include "AsyncClientLib/Session/session_strategy.h"

class onHelloStrategy : public SessionStrategy
{
public:
    onHelloStrategy();
    void run(std::weak_ptr<Session> session, std::shared_ptr<char []> payload_packet_data) override;

private:
    CryptoPP::word32 m_rand_num;
    bool wait_for_confirm;

};
