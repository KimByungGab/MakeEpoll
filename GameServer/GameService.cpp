#include "GameService.h"

void GameService::Init(const int maxClientSize)
{
    m_userManager = make_unique<UserManager>();
    m_userManager->Init(maxClientSize);
}

void GameService::LoginProcess(shared_ptr<Client> client, const string& ip)
{
    if(client == nullptr)
        return;

    shared_ptr<User> user = m_userManager->GetUser(client->GetFD());
    if(user == nullptr)
        return;

    user->LogIn(ip, client);
}

void GameService::LogoutProcess(const int clientIndex)
{
    shared_ptr<User> user = m_userManager->GetUser(clientIndex);
    if(user == nullptr)
        return;

    user->LogOut();
}

void GameService::Parsing(WaitPacketData& waitPacketData)
{
    unsigned short packetID;

    DataArchive ar;
    ar.SetBuffer(waitPacketData.GetData(), waitPacketData.GetDataSize());
    ar >> packetID;

    // 어떤 유저가 사용했는지 확인
    int clientIndex = waitPacketData.GetClientIndex();
    if(clientIndex < 0 || clientIndex >= m_userManager->GetMaxUserSize())
        return;

    shared_ptr<User> user = m_userManager->GetUser(clientIndex);
    if(user == nullptr || !user->IsLogIn())
        return;

    vector<WaitPacketData> sendPackets;

    // 패킷 헤더 파싱
    switch(static_cast<PacketID>(packetID))
    {
        case PacketID::PACKET_ID_CHAT_ECHO_MESSAGE: // 예시 패킷
            RCV_ChatEchoMessage(user, ar);
            break;
        default:
            break;
    }
}

void GameService::RCV_ChatEchoMessage(shared_ptr<User> sourceUser, DataArchive& ar)
{
    PacketRecvChat packet;
    packet.Deserialize(ar);

    if(sourceUser == nullptr)
        return;

    sourceUser->ChatMessage(packet.message);
}