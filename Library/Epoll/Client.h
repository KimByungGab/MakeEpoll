#pragma once

#include "RingBuffer.h"
#include <queue>

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
    Client(int recvBufferSize);

    bool IsConnected() { return m_ConnectType == eConnectType::Connect ? true : false; }
    bool IsDisConnected() { return m_ConnectType == eConnectType::Disconnect ? true : false; }
    bool Connect(int epollFDIndex);
    bool Disconnect();
    void WriteRecvPacket(char* buffer, int size);
    void ReadRecvBuffer(char* buffer, int size);
    void PushSendPacket(char* buffer, int size) { m_sendQueue.push(vector<char>(buffer, buffer + size)); }
    bool PopSendPacket(vector<char>& outBuffer);
    int GetEpollFDIndex() { return m_epollFDIndex; }

private:
    void Init();

private:
    int m_epollFDIndex;

    eConnectType m_ConnectType;

    RingBuffer m_recvBuf;
    queue<vector<char>> m_sendQueue;
};