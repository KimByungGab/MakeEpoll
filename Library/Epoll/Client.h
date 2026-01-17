#pragma once

#include "RingBuffer.h"
#include <queue>
#include <vector>
#include <mutex>
#include <memory>

class NetworkCore;

using namespace std;

enum class eConnectType
{
    Disconnect = 0,
    Connect = 1
};

enum class eRecvSend
{
    Recv = 0,
    Send = 1,
};

class Client
{
public:
    Client(int FD, int recvBufferSize, weak_ptr<NetworkCore> networkCore) : m_FD(FD), m_recvBuf(recvBufferSize), m_networkCore(networkCore) { Init(); }

    bool IsConnected() { return m_ConnectType == eConnectType::Connect ? true : false; }
    bool IsDisConnected() { return m_ConnectType == eConnectType::Disconnect ? true : false; }
    bool Connect(int epollFDIndex);
    bool Disconnect();
    void WriteRecvPacket(char* buffer, int size);
    void ReadRecvBuffer(char* buffer, int size);
    void PushSendPacket(char* buffer, int size);
    bool PopSendPacket(vector<char>& outBuffer);
    int GetEpollFDIndex() { return m_epollFDIndex; }

    int GetFD() { return m_FD; }
    void SendData(char* buffer, int size);

private:
    void Init();

private:
    int m_FD;
    int m_epollFDIndex;

    eConnectType m_ConnectType;

    RingBuffer m_recvBuf;
    queue<vector<char>> m_sendQueue;
    mutex m_sendMutex;

    weak_ptr<NetworkCore> m_networkCore;
};