#pragma once

#include "PacketDef.h"

class PacketLogin : public IPacketDeserialize
{
public:
    void Deserialize(DataArchive& ar) override
    {
        ar >> ip >> clientIndex;
    }

public:
    string ip;
    int clientIndex;
};