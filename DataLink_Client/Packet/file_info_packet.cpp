#include "file_info_packet.h"

FileInfoPacket::FileInfoPacket()
{

}

FileInfoPacket::~FileInfoPacket()
{

}

std::shared_ptr<std::string> FileInfoPacket::makePacket(std::string file_name,
                                                        int file_size, std::string checksum)
{
    // Make an unencrypted TestPacket in buf
    unsigned int buf_len = file_name.length() + checksum.length() + sizeof(FileInfoPacket);
    std::shared_ptr<unsigned char[]> buf(new unsigned char[buf_len]);
    FileInfoPacket* packet = reinterpret_cast<FileInfoPacket*>(buf.get());
    packet->setType(Type::INFO);
    packet->setFileSize(file_size);
    packet->setFileName(file_name);
    packet->setChecksum(checksum);

    // Construct TestPacket
    std::shared_ptr<std::string> ret =
        std::make_shared<std::string>(reinterpret_cast<const char*>(packet), buf_len);

    return ret;
}

std::string FileInfoPacket::getFileName() const
{
    return std::string(m_data, m_file_name_len);
}

std::string FileInfoPacket::getFileCheckSum() const
{
    return std::string(m_data + m_file_name_len, m_checksum_len);
}

int FileInfoPacket::getFileSize() const
{
    return m_file_size;
}

void FileInfoPacket::setFileSize(int len)
{
    m_file_size = len;
}

void FileInfoPacket::setFileName(std::string file_name)
{
    m_file_name_len = file_name.length();
    memcpy_s(m_data, file_name.length(), file_name.c_str(), file_name.length());
}

void FileInfoPacket::setChecksum(std::string checksum)
{
    m_checksum_len = checksum.length();
    memcpy_s(m_data + m_file_name_len, checksum.length(), checksum.c_str(), checksum.length());
}
