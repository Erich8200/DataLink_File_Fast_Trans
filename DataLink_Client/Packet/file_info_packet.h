#ifndef FILEINFOPACKET_H
#define FILEINFOPACKET_H

#include "file_packet.h"

class FileInfoPacket : public FilePacket
{
public:
    FileInfoPacket();
    ~FileInfoPacket();
    static std::shared_ptr<std::string> makePacket(std::string file_name,
                                                   int file_size,
                                                   std::string checksum);

    // get...
    std::string getFileName() const;
    std::string getFileCheckSum() const;
    int getFileSize() const;

    // set...
    void setFileSize(int len);
    void setFileName(std::string file_name);
    void setChecksum(std::string checksum);

private:
    int                      m_file_name_len;
    int                      m_file_size;
    int                      m_checksum_len;
    char                     m_data[0];

};

#endif // FILEINFOPACKET_H
