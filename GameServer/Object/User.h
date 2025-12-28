#pragma once

#include <iostream>
#include <string>

#include "WaitPacketData.h"

#include "Packet/PacketRecvChat.h"
#include "Packet/PacketSendChat.h"

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
    
    void LogIn(const string& ip, const int userFD);
    void LogOut();
    bool IsLogIn() { return m_userState == eUserState::LogIn ? true : false; }

    //==========================================================================
    // 수신 패킷 처리 함수들
    //==========================================================================
    vector<WaitPacketData> RCV_ChatMessage(DataArchive& ar);

    //==========================================================================
    // 송신 패킷 처리 함수들
    //==========================================================================

private:
    eUserState m_userState;
    string m_IP;
    int m_userFD;
};