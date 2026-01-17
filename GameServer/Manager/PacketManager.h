#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Object/WaitPacketData.h"
#include "GameService.h"

#include "Library/Epoll/Client.h"

#include "Packet/PacketDef.h"
#include "Packet/PacketRecvChat.h"

using namespace std;

class PacketManager
{
public:
    void Init(const int maxClientSize);
    void Run();
    void End();

    void PushData(const int clientIndex, char* buffer, int bufferSize);
    void SendData(const int clientIndex, DataArchive& ar);

    void LoginProcess(shared_ptr<Client> client, const string& ip) { m_gameService->LoginProcess(client, ip); }
    void LogoutProcess(const int clientIndex) { m_gameService->LogoutProcess(clientIndex); }

private:
    void PacketManagerThread();

private:
    queue<WaitPacketData> m_packetQueue;
    bool m_isRunPacketThread = false;
    vector<thread> m_packetThreads;
    mutex m_packetProcessMutex;
    condition_variable m_packetCV;

    unique_ptr<GameService> m_gameService;
};