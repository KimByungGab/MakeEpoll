#include "Client.h"
#include "NetworkCore.h"

bool Client::Connect(int epollFDIndex)
{
    if(IsConnected())
        return false;
    
    m_ConnectType = eConnectType::Connect;
    m_epollFDIndex = epollFDIndex;

    return true;
}

bool Client::Disconnect()
{
    if(IsDisConnected())
        return false;

    Init();
    
    return true;
}

void Client::WriteRecvPacket(char* buffer, int size)
{
    m_recvBuf.Write(buffer, size);
}

void Client::ReadRecvBuffer(char* buffer, int size)
{
    m_recvBuf.Read(buffer, size);
}

void Client::Init()
{
    m_ConnectType = eConnectType::Disconnect;
    m_epollFDIndex = -1;
}

void Client::PushSendPacket(char* buffer, int size)
{
    lock_guard<mutex> lock(m_sendMutex);
    m_sendQueue.push(vector<char>(buffer, buffer + size));
}

bool Client::PopSendPacket(vector<char>& outBuffer)
{
    if(m_sendQueue.empty())
        return false;

    lock_guard<mutex> lock(m_sendMutex);
    outBuffer = m_sendQueue.front();
    m_sendQueue.pop();

    return true;
}

void Client::SendData(char* buffer, int size)
{
    PushSendPacket(buffer, size);

    shared_ptr<NetworkCore> networkCore = m_networkCore.lock();
    if(networkCore == nullptr)
        return;

    networkCore->EnableWrite(m_FD, m_epollFDIndex);
}