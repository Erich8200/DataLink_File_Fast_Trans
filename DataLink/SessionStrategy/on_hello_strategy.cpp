#include "on_hello_strategy.h"
#include "../my_global.h"
#include "AsyncServer_VS/Packet/packet.h"
#include "../Packet/hello_packet.h"

onHelloStrategy::onHelloStrategy()
{
    m_challenged = false;
}

void onHelloStrategy::run(std::weak_ptr<Session> session,std::shared_ptr<char[]> payload_packet_data)
{
    unsigned long long id = session.lock()->getID();
    std::shared_ptr<HelloPacket> packet = std::reinterpret_pointer_cast<HelloPacket>(payload_packet_data);
    if (!m_challenged) { // 待挑战,不看接收到的包，发送挑战包
        if (!session.expired()) {
            m_rand_num = CryptoUtils::get_rand_word32();
            std::string challenge_packet_str = HelloPacket::makeHelloPacket(m_rand_num);
            std::string packet_str = Packet::makePacket(challenge_packet_str, PacketType::HELLO);
            session.lock()->Send(packet_str);
        }
        m_challenged = true;
    } else { // 挑战包已经发出，期待正确的答复包（m_rand_num + 1）
        if (packet->getNum() == m_rand_num + 1) {

            /* 发送挑战通过确认包（m_rand_num + 2） */
            std::string challenge_packet_str = HelloPacket::makeHelloPacket(m_rand_num + 2);
            std::string packet_str = Packet::makePacket(challenge_packet_str, PacketType::HELLO);
            session.lock()->Send(packet_str);

            /* 设置模型 */
            std::lock_guard<std::mutex> g(g_conn_model_mutex);
            g_conn_model->item(getRowFromModelById(g_conn_model, id, ConnTableColumn::ID),
                               ConnTableColumn::Valid)->setData(true);

        } else {
            if (!session.expired()) {
                std::lock_guard<std::mutex> g(g_conn_model_mutex);
                g_conn_model->item(getRowFromModelById(g_conn_model, id, ConnTableColumn::ID),
                                   ConnTableColumn::Valid)->setData(false);
                session.lock()->setShouldClose(true);
            }
        }
    }
}
