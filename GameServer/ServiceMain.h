#pragma once

#include "Manager/PacketManager.h"
#include "Library/Epoll/NetworkCore.h"
#include "Library/DataArchive/DataArchive.h"

class ServiceMain : public NetworkCore
{
public:
    ServiceMain(_SERVER_INFO serverInfo);

    virtual void OnConnect(shared_ptr<Client> client, string ip) override;
    virtual void OnClose(const int clientIndex) override;
    virtual void OnReceive(const int clientIndex, char* buffer, int bufferSize) override;

    void Run();
    void End();

private:
    unique_ptr<PacketManager> m_packetManager;
};