#include "PacketManager.h"

void PacketManager::PushData(const int clientIndex, char* buffer, int bufferSize)
{    
    WaitPacketData waitPacketData;
    waitPacketData.SetData(clientIndex, buffer, bufferSize);
    
    {
        lock_guard<mutex> lock(m_packetProcessMutex);
        m_packetQueue.push(std::move(waitPacketData));
    }

    m_packetCV.notify_one();
}

void PacketManager::Init(const int maxClientSize, IPacketSender* packetSender)
{
    m_userManager = make_unique<UserManager>();
    m_userManager->Init(maxClientSize);

    m_packetSender = packetSender;
}

void PacketManager::Run()
{
    m_isRunPacketThread = true;
    for(int i = 0; i < thread::hardware_concurrency(); i++)
    {
        m_packetThreads.emplace_back(thread([this]()
            {
                PacketManagerThread();
            }));
    }
}

void PacketManager::End()
{
    m_isRunPacketThread = false;
    m_packetCV.notify_all();
    for(int i = 0; i < m_packetThreads.size(); i++)
    {
        if(m_packetThreads[i].joinable())
            m_packetThreads[i].join();
    }
}

void PacketManager::PacketManagerThread()
{
    WaitPacketData waitPacketData;

    while(m_isRunPacketThread)
    {
        WaitPacketData waitPacketData;
        {
            unique_lock<mutex> lock(m_packetProcessMutex);
            m_packetCV.wait(lock, [this]() {
                 return !m_packetQueue.empty() || !m_isRunPacketThread; 
                });

            if(!m_isRunPacketThread && m_packetQueue.empty())
                break;
            
            waitPacketData = std::move(m_packetQueue.front());
            m_packetQueue.pop();
        }
        
        Parsing(waitPacketData);
    }
}

void PacketManager::Parsing(WaitPacketData& waitPacketData)
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
        case PacketID::PACKET_ID_CHAT_MESSAGE: // 예시 패킷
            sendPackets = user->RCV_ChatMessage(ar);
            break;
        default:
            break;
    }

    for(auto& sendPacket : sendPackets)
        m_packetSender->SendPacket(sendPacket.GetClientIndex(), sendPacket.GetData(), sendPacket.GetDataSize());
}

void PacketManager::LoginProcess(const int clientIndex, const string& ip)
{
    shared_ptr<User> user = m_userManager->GetUser(clientIndex);
    if(user == nullptr)
        return;
    
    user->LogIn(ip, clientIndex);
}

void PacketManager::LogoutProcess(const int clientIndex)
{
    shared_ptr<User> user = m_userManager->GetUser(clientIndex);
    if(user == nullptr)
        return;

    user->LogOut();
}