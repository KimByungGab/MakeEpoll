#include "ServiceMain.h"

ServiceMain::ServiceMain(_SERVER_INFO serverInfo) : NetworkCore(serverInfo)
{
    
}

void ServiceMain::OnConnect(shared_ptr<Client> client, string ip)
{
    if(client == nullptr)
        return;

    cout << "LogIn! IP: " << ip << "(" << client->GetFD() << ")" << endl;
    m_packetManager->LoginProcess(client, ip);
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

void ServiceMain::Run()
{
    m_packetManager = make_unique<PacketManager>();
    m_packetManager->Init(m_serverInfo.maxClientSize);
    m_packetManager->Run();
    
    StartServer();
}

void ServiceMain::End()
{
    m_packetManager->End();

    EndServer();
}