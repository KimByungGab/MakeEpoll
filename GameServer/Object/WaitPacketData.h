#pragma once

#include <vector>

using namespace std;

class WaitPacketData
{
public:
    void SetData(int clientIndex, char* packet, int size);
    char* GetData() { return m_packet.data(); }
    int GetDataSize() { return m_packet.size(); }
    int GetClientIndex() { return m_clientIndex; }

private:
    int m_clientIndex;
    vector<char> m_packet;
};