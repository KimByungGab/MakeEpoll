#pragma once

class IPacketSender
{
public:
    virtual ~IPacketSender() = default;
    virtual void SendPacket(const int clientIndex, const char* data, const int size) = 0;
};