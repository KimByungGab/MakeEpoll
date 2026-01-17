#include "User.h"

User::User()
{
    m_userState = eUserState::LogOut;
    m_IP.clear();
    m_client = nullptr;
}

void User::LogIn(const string& ip, shared_ptr<Client> client)
{
    m_userState = eUserState::LogIn;
    m_IP = ip;
    m_client = client;
}

void User::LogOut()
{
    m_userState = eUserState::LogOut;
    m_IP.clear();
    m_client = nullptr;
}

void User::ChatMessage(const string& message)
{
    if(m_client == nullptr)
        return;

    cout << "User(" << m_client->GetFD() << "): " << message << endl;

    // 에코
    DataArchive sendAr;
    PacketSendChat sendDTO(message);
    sendDTO.Serialize(sendAr);

    m_client->SendData(sendAr.GetBufferToPtr(), sendAr.GetBufferSize());
}