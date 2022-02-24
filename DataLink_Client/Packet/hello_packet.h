#pragma once

#include <string>
#include <eccrypto.h>

#pragma pack(push, 1)

/*
	This packet is used for challenge, but the data may be used for storing
	something interesting, such as host info
*/

class HelloPacket
{
public:
	HelloPacket();
	~HelloPacket();
	void setNum(CryptoPP::word32 num);
	CryptoPP::word32 getNum() const;

	// Packet serialization
	// Returns a HelloPacket in a std::string pointer
	static std::string makeHelloPacket(CryptoPP::word32 rand_num);

private:
	CryptoPP::word32             m_rand_num;
};

#pragma pack(pop)