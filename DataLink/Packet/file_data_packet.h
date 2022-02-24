#ifndef FILEDATAPACKET_H
#define FILEDATAPACKET_H

#include "file_packet.h"

#pragma pack(push, 1)

class FileDataPacket : public FilePacket
{
public:
    FileDataPacket();
    ~FileDataPacket();

    void setLastPiece(bool last_piece);
    void setData(const unsigned char* data, unsigned int len, unsigned buf_len);
    unsigned char* getData();
    unsigned int getDataLen() const;
    bool getIsLastPiece() const;

    // Packet serialization
    // Returns a TestPacket in a std::string pointer
    static std::shared_ptr<std::string> makePacket(const unsigned char* data, unsigned int data_len, bool is_last);

private:
    bool                     m_is_last_piece;
    unsigned int             m_data_len = 0;
    unsigned char            m_data[0];
};

#pragma pack(pop)

#endif // FILEDATAPACKET_H
