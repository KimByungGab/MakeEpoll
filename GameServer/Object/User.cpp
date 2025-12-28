#include "User.h"

User::User()
{
    m_userState = eUserState::LogOut;
    m_IP.clear();
    m_userFD = -1;
}

void User::LogIn(const string& ip, const int userFD)
{
    m_userState = eUserState::LogIn;
    m_IP = ip;
    m_userFD = userFD;
}

void User::LogOut()
{
    m_userState = eUserState::LogOut;
    m_IP.clear();
    m_userFD = -1;
}

vector<WaitPacketData> User::RCV_ChatMessage(DataArchive& ar)
{
    PacketRecvChat packet;
    packet.Deserialize(ar);

    cout << "User(" << m_userFD << "): " << packet.message << endl;

    // 에코
    DataArchive sendAr;
    PacketSendChat sendDTO(packet.message);
    sendDTO.Serialize(sendAr);

    vector<WaitPacketData> sendPackets;
    WaitPacketData waitPacketData;
    waitPacketData.SetData(m_userFD, sendAr.GetBufferToPtr(), sendAr.GetBufferSize());
    sendPackets.push_back(waitPacketData);

    return sendPackets;
}