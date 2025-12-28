#include "WaitPacketData.h"

void WaitPacketData::SetData(int clientIndex, char* packet, int size)
{
    m_clientIndex = clientIndex;
    m_packet.insert(m_packet.begin(), packet, packet + size);
}