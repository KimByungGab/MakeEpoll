#pragma once

#include "PacketDef.h"

class PacketRecvChat : public IPacketDeserialize
{
public:
    void Deserialize(DataArchive& ar) override
    {
        ar >> message;
    }

public:
    string message;
};