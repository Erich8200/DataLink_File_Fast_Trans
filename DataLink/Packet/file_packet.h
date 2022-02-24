#pragma once

#include <string>

#pragma pack(push, 1)
class FilePacket
{
public:
    FilePacket();
    ~FilePacket();

    enum Type{INFO, DATA};

    void setType(Type t);
    Type getType();

protected:
    Type m_type;
};

#pragma pack(pop)
