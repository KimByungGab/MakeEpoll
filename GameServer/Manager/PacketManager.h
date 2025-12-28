#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Define.h"

#include "Object/WaitPacketData.h"
#include "UserManager.h"

#include "Library/Singleton/Singleton.h"

#include "Packet/PacketDef.h"
#include "Packet/PacketRecvChat.h"

using namespace std;

class PacketManager
{
public:
    void Init(const int maxClientSize, IPacketSender* packetSender);
    void Run();
    void End();

    void PushData(const int clientIndex, char* buffer, int bufferSize);
    void SendData(const int clientIndex, DataArchive& ar);

    void LoginProcess(const int clientIndex, const string& ip);
    void LogoutProcess(const int clientIndex);

private:
    void PacketManagerThread();
    void Parsing(WaitPacketData& waitPacketData);

private:
    queue<WaitPacketData> m_packetQueue;
    bool m_isRunPacketThread = false;
    vector<thread> m_packetThreads;
    mutex m_packetProcessMutex;
    condition_variable m_packetCV;

    unique_ptr<UserManager> m_userManager;

    IPacketSender* m_packetSender = nullptr;
};