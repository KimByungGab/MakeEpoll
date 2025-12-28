#include "ServiceMain.h"

ServiceMain::ServiceMain(_SERVER_INFO serverInfo) : NetworkCore(serverInfo)
{
    
}

void ServiceMain::OnConnect(const int clientIndex, string ip)
{
    cout << "LogIn! IP: " << ip << "(" << clientIndex << ")" << endl;
    m_packetManager->LoginProcess(clientIndex, ip);
}

void ServiceMain::OnClose(const int clientIndex)
{
    cout << "클라이언트 종료! FD: " << clientIndex << endl;
    m_packetManager->LogoutProcess(clientIndex);
}

void ServiceMain::OnReceive(const int clientIndex, char* buffer, int bufferSize)
{
    m_packetManager->PushData(clientIndex, buffer, bufferSize);
}

void ServiceMain::SendPacket(const int clientIndex, const char* data, const int size)
{
    PushSendData(clientIndex, const_cast<char*>(data), size);
}

void ServiceMain::Run()
{
    m_packetManager = make_unique<PacketManager>();
    m_packetManager->Init(m_serverInfo.maxClientSize, this);
    m_packetManager->Run();
    
    StartServer();
}

void ServiceMain::End()
{
    m_packetManager->End();

    EndServer();
}