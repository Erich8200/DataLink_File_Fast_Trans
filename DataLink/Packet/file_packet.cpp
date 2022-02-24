#include "file_packet.h"

FilePacket::FilePacket()
{
}

FilePacket::~FilePacket()
{
}

void FilePacket::setType(Type t)
{
    m_type = t;
}

FilePacket::Type FilePacket::getType()
{
    return m_type;
}


