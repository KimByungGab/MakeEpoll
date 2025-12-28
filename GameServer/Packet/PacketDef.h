#pragma once

#include <iostream>
#include "Library/DataArchive/DataArchive.h"

using namespace std;

enum class PacketID : unsigned short
{
    PACKET_ID_NONE = 0,
    
    PACKET_ID_LOGIN = 1,
    PACKET_ID_LOGOUT = 2,

    PACKET_ID_CHAT_MESSAGE = 3,
};

struct PacketHeader
{
    unsigned int magicNumber1 = 0x12345678;
    unsigned int magicNumber2 = 0x87654321;
    unsigned short packetID;
};

class PacketSerialize : public PacketHeader
{
public:
    virtual void Serialize(DataArchive& ar) = 0;

public:
    PacketHeader header;
};

class IPacketDeserialize
{
public:
    virtual void Deserialize(DataArchive& ar) = 0;
};