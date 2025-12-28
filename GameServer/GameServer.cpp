#include <iostream>

#include "ServiceMain.h"

#include "Library/Epoll/Define.h"
#include "Library/Ini/Ini.h"

int main()
{
    Ini ini;
    ini.loadIni("GameServer.ini");

    int port = stoi(ini.FindSectionData("Server", "PORT"));
    int maxEvent = stoi(ini.FindSectionData("Server", "MAXEVENT"));
    int bufferSize = stoi(ini.FindSectionData("Server", "BUFFERSIZE"));
    int timeout = stoi(ini.FindSectionData("Server", "TIMEOUT"));
    int epollThreadCount = stoi(ini.FindSectionData("Server", "EPOLLTHREADCOUNT"));
    int workerThreadCount = stoi(ini.FindSectionData("Server", "WORKERTHREADCOUNT"));
    int maxClientSize = stoi(ini.FindSectionData("Server", "MAXCLIENTSIZE"));

    _SERVER_INFO serverInfo;
    serverInfo.mode = SOCKET_MODE::REUSE_ADDR;
    serverInfo.port = port;
    serverInfo.recvBufSize = bufferSize;
    serverInfo.eventSize = maxEvent;
    serverInfo.timeout = timeout;
    serverInfo.epollThreadCount = epollThreadCount;
    serverInfo.workerThreadCount = workerThreadCount;
    serverInfo.maxClientSize = maxClientSize;


    ServiceMain serviceMain(serverInfo);
    serviceMain.Run();

    cout << "종료: quit" << endl;

    // TODO(KBG): 다른 프로세스로 간섭하여 종료시킬 수 있는 프로그램을 만들기
    while(true)
    {
        string input;
        getline(cin, input);

        if(input == "/quit")
            break;
    }

    serviceMain.End();
    
    return 0;
}