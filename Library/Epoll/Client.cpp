#include "Client.h"

Client::Client(int recvBufferSize) : m_recvBuf(recvBufferSize)
{
    Init();
}

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

bool Client::PopSendPacket(vector<char>& outBuffer)
{
    if(m_sendQueue.empty())
        return false;

    outBuffer = m_sendQueue.front();
    m_sendQueue.pop();

    return true;
}