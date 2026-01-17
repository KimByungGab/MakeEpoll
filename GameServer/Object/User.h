#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "WaitPacketData.h"

#include "Packet/PacketRecvChat.h"
#include "Packet/PacketSendChat.h"

#include "Library/Epoll/Client.h"

using namespace std;

class PacketManager;

enum class eUserState
{
    LogOut = 0,
    LogIn = 1,
};

class User
{
public:
    User();
    
    void LogIn(const string& ip, shared_ptr<Client> client);
    void LogOut();
    bool IsLogIn() { return m_userState == eUserState::LogIn ? true : false; }

    void ChatMessage(const string& message);

private:
    eUserState m_userState;
    string m_IP;
    
    shared_ptr<Client> m_client;
};