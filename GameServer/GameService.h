#pragma once

#include <iostream>
#include <memory>

#include "Manager/UserManager.h"

#include "Object/User.h"
#include "Object/WaitPacketData.h"

#include "Library/DataArchive/DataArchive.h"

#include "Packet/PacketDef.h"
#include "Packet/PacketRecvChat.h"
#include "Packet/PacketSendChat.h"

using namespace std;

class GameService
{
public:
    void Init(const int maxClientSize);

    void LoginProcess(shared_ptr<Client> client, const string& ip);
    void LogoutProcess(const int clientIndex);

    void Parsing(WaitPacketData& waitPacketData);

    //==========================================================================
    // 수신 패킷 처리 함수들
    //==========================================================================
    void RCV_ChatEchoMessage(shared_ptr<User> sourceUser, DataArchive& ar);

private:
    unique_ptr<UserManager> m_userManager;
};