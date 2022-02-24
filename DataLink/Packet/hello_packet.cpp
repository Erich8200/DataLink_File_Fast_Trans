#include "hello_packet.h"

HelloPacket::HelloPacket()
{
	m_rand_num = 0;
}

HelloPacket::~HelloPacket()
{
}

void HelloPacket::setNum(CryptoPP::word32 num)
{
	m_rand_num = num;
}

CryptoPP::word32 HelloPacket::getNum() const
{
	return m_rand_num;
}

std::string HelloPacket::makeHelloPacket(CryptoPP::word32 rand_num)
{
	return std::string(reinterpret_cast<const char*>(&rand_num), sizeof(m_rand_num));
}
