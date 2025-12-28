#pragma once

#include "Define.h"

#include "Packet/PacketLogin.h"
#include "Manager/PacketManager.h"
#include "Library/Epoll/NetworkCore.h"
#include "Library/DataArchive/DataArchive.h"

class ServiceMain : public NetworkCore, public IPacketSender
{
public:
    ServiceMain(_SERVER_INFO serverInfo);

    virtual void OnConnect(const int clientIndex, string ip) override;
    virtual void OnClose(const int clientIndex) override;
    virtual void OnReceive(const int clientIndex, char* buffer, int bufferSize) override;

    virtual void SendPacket(const int clientIndex, const char* data, const int size) override;

    void Run();
    void End();

private:
    unique_ptr<PacketManager> m_packetManager;
};