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

void PacketManager::Init(const int maxClientSize)
{
    m_gameService = make_unique<GameService>();
    m_gameService->Init(maxClientSize);
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

        m_gameService->Parsing(waitPacketData);
    }
}