#include "on_hello_strategy.h"
#include "../my_global.h"
#include "AsyncClientLib/Packet/packet.h"
#include "../Packet/hello_packet.h"
#include "AsyncClientLib/Session/session_manager.h"

onHelloStrategy::onHelloStrategy()
{
    m_rand_num = 0;
    wait_for_confirm = false;
}

void onHelloStrategy::run(std::weak_ptr<Session> session,std::shared_ptr<char[]> payload_packet_data)
{
    typedef class HelloPacket ChallengePacket;
    unsigned long long id = session.lock()->getID();

    const ChallengePacket* recvd_challenge_packet = reinterpret_cast<const ChallengePacket*>(payload_packet_data.get());

    if (wait_for_confirm) {
        // 第二次接收到HelloPacket，如果服务器返回的值是rand+2，就设置已通过服务器端的挑战
        if (recvd_challenge_packet->getNum() == m_rand_num + 2) {
            std::lock_guard<std::mutex> g(g_session_validation_states_mutex);
            g_session_validation_states[id].store(true, std::memory_order_release);
        }
    } else {
        // 第一次接收到HelloPacket，回答并返回挑战答复包
        m_rand_num = recvd_challenge_packet->getNum();
        wait_for_confirm = true;
        CryptoPP::word32 num = m_rand_num + 1;
        auto hello_packet_str = HelloPacket::makeHelloPacket(num);
        auto hello_packet_full_str = Packet::makePacket(hello_packet_str, PacketType::HELLO);
        std::shared_ptr<std::string> hello_packet_full_str_ptr = std::make_shared<std::string>(hello_packet_full_str);
        g_session_manager->SessionSend(id, hello_packet_full_str_ptr);
    }
}
