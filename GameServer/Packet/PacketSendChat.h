#pragma once

#include "PacketDef.h"

class PacketSendChat : public PacketSerialize
{
public:
    PacketSendChat(string message)
    {
        header.packetID = static_cast<unsigned short>(PacketID::PACKET_ID_CHAT_MESSAGE);
        this->message = message;
    }

    void Serialize(DataArchive& ar) override
    {
        ar << header.magicNumber1 << header.magicNumber2 << header.packetID;
        ar << message;
    }

public:
    std::string message;
};