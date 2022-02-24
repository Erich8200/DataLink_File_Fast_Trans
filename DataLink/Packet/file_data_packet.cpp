#include "file_data_packet.h"

FileDataPacket::FileDataPacket()
{

}

FileDataPacket::~FileDataPacket()
{

}

void FileDataPacket::setLastPiece(bool last_piece)
{
    m_is_last_piece = last_piece;
}

void FileDataPacket::setData(const unsigned char * data, unsigned int len, unsigned buf_len)
{
    m_data_len = len;
    memcpy_s(m_data, buf_len, data, len);
}

unsigned char * FileDataPacket::getData()
{
    return m_data;
}

unsigned int FileDataPacket::getDataLen() const
{
    return m_data_len;
}

bool FileDataPacket::getIsLastPiece() const
{
    return m_is_last_piece;
}

std::shared_ptr<std::string> FileDataPacket::makePacket(const unsigned char * data, unsigned int data_len, bool is_last)
{
    // Make an unencrypted TestPacket in buf
    unsigned int buf_len = data_len + sizeof(FileDataPacket);
    std::shared_ptr<unsigned char[]> buf(new unsigned char[buf_len]);
    FileDataPacket* packet = reinterpret_cast<FileDataPacket*>(buf.get());
    packet->setType(Type::DATA);
    packet->setLastPiece(is_last);
    packet->setData(data, data_len, data_len);

    // Construct TestPacket
    std::shared_ptr<std::string> ret =
        std::make_shared<std::string>(reinterpret_cast<const char*>(packet), buf_len);

    return ret;
}
