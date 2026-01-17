#include "NetworkCore.h"

NetworkCore::NetworkCore(_SERVER_INFO serverInfo)
{
    Init();
    m_serverInfo = serverInfo;
}

void NetworkCore::StartServer()
{
    if(m_serverInfo.port <= 0 || m_serverInfo.port >= 36656)
    {
        cout << "포트 길이가 범위 밖에 있습니다. (포트번호: " << m_serverInfo.port << ")" << endl;
        return;
    }

    if(m_serverInfo.maxSize <= 0 || m_serverInfo.maxSize > SOMAXCONN)
    {
        cout << "소켓 리슨 사이즈가 범위 밖에 있습니다. 최대 소켓 갯수로 조정합니다. (사이즈: " << m_serverInfo.maxSize << ")" << endl;
        m_serverInfo.maxSize = SOMAXCONN;
    }

    if(m_serverInfo.eventSize <= 0 || m_serverInfo.eventSize > MAX_EVENT_SIZE)
    {
        cout << "Epoll 이벤트 사이즈가 범위 밖에 있습니다. 최대 이벤트 사이즈로 조정합니다. (사이즈: " << m_serverInfo.eventSize << ")" << endl;
        m_serverInfo.eventSize = MAX_EVENT_SIZE;
    }

    if(m_serverInfo.timeout < -1 || m_serverInfo.timeout > 1000)
    {
        cout << "timeout이 범위 밖에 있습니다. 동기 소켓 버전으로 timeout을 지정합니다. (timeout: " << m_serverInfo.timeout << ")" << endl;
        m_serverInfo.timeout = -1;
    }

    if(m_serverInfo.recvBufSize <= 0 || m_serverInfo.recvBufSize > MAX_BUFFER_SIZE)
    {
        cout << "버퍼 사이즈가 범위 밖에 있습니다. 최대 버퍼 사이즈로 조정합니다. (사이즈: " << m_serverInfo.recvBufSize << ")" << endl;
        m_serverInfo.recvBufSize = MAX_BUFFER_SIZE;
    }

    unsigned int currentEquipmentThreadCount = thread::hardware_concurrency();
    if(m_serverInfo.epollThreadCount <= 0 || m_serverInfo.epollThreadCount > currentEquipmentThreadCount)
    {
        cout << "Epoll 네트워크 I/O 쓰레드의 설정 갯수가 범위 밖에 있습니다. 하드웨어 쓰레드 수로 조정합니다. (설정수: " << m_serverInfo.epollThreadCount << ")" << endl;
        m_serverInfo.epollThreadCount = currentEquipmentThreadCount;
    }
    
    if(CreateSocket() == false)
        return;
    
    SetNonBlocking(m_listenFD);
    
    if(BindSocket() == false)
        return;

    if(ListenSocket() == false)
        return;

    StartEpoll();
}

void NetworkCore::EndServer()
{
    // Accept Thread 종료
    m_isAcceptRun = false;
    if(m_acceptThread.joinable())
        m_acceptThread.join();
    
    // 서버 루틴 종료
    m_isWorkerRun = false;
    for(int i = 0; i < m_workerThreads.size(); i++)
    {
        if(m_workerThreads[i].joinable())
            m_workerThreads[i].join();
    }
}

void NetworkCore::Init()
{
    m_listenFD = -1;
}

bool NetworkCore::CreateSocket()
{
    m_listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if(m_listenFD < 0)
    {
        cout << "소켓을 생성할 수 없습니다." << endl;
        return false;
    }

    switch(m_serverInfo.mode)
    {
        case SOCKET_MODE::REUSE_ADDR:
            SetReuseAddr(m_listenFD);
            break;
        default:
            break;
    }

    return true;
}

bool NetworkCore::BindSocket()
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_serverInfo.port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(m_listenFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        cout << "소켓을 바인드 할 수 없습니다." << endl;
        return false;
    }

    return true;
}

bool NetworkCore::ListenSocket()
{
    if(listen(m_listenFD, m_serverInfo.maxSize) == -1)
    {
        cout << "소켓을 리슨할 수 없습니다." << endl;
        return false;
    }

    return true;
}

void NetworkCore::SetReuseAddr(int& FD)
{
    int opt = 1;
    setsockopt(FD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void NetworkCore::SetNonBlocking(int& FD)
{
    int flags = fcntl(FD, F_GETFL, 0);
    fcntl(FD, F_SETFL, flags | O_NONBLOCK);
}

void NetworkCore::StartEpoll()
{
    SetEpollFDs();
    InitClientInfo();

    if(CreateAcceptThread(m_serverInfo.timeout, m_serverInfo.eventSize) == false)
    {
        cout << "Accept Thread가 시작되지 못했습니다. 프로그램을 종료합니다." << endl;
        return;
    }

    if(CreateServerThread(m_serverInfo.timeout, m_serverInfo.eventSize) == false)
    {
        cout << "서버를 시작하지 못했습니다. 프로그램을 종료합니다." << endl;
        return;
    }
}

bool NetworkCore::CreateAcceptThread(int timeout, int eventSize)
{
    m_isAcceptRun = true;
    m_acceptThread = thread([this, timeout, eventSize]()
        {
            AcceptThread(timeout, eventSize);
        });

    cout << "AcceptThread 시작!" << endl;
    return true;
}

void NetworkCore::AcceptThread(int timeout, int eventSize)
{
    epoll_event events[eventSize];

    while(m_isAcceptRun)
    {
        int n = epoll_wait(m_acceptEpollFD, events, eventSize, timeout);
        
        for(int i = 0; i < n; i++)
        {
            if(events[i].events & EPOLLIN)
            {
                while(true)
                {
                    sockaddr_in clientAddr;
                    socklen_t len = sizeof(clientAddr);
                    int clientFD = accept(m_listenFD, (sockaddr*)&clientAddr, &len);

                    // 더 이상 accept 할 게 없음
                    if(clientFD == -1)
                        break;

                    // epollFD에게 관심사 추가
                    int minCountIndex = 0;
                    int minCount = m_workerClientCount[0];
                    for(int j = 0; j < m_workerClientCount.size(); j++)
                    {
                        if(minCount > m_workerClientCount[j])
                        {
                            minCount = m_workerClientCount[j];
                            minCountIndex = j;
                        }
                    }

                    if(ConnectClientInfo(clientFD, minCountIndex) == false)
                    {
                        cout << "클라이언트가 최대치에 도달하여 접속할 수 없습니다. (FD: " << clientFD << ")" << endl;
                        close(clientFD);
                        continue;
                    }

                    // 논블로킹 모드
                    SetNonBlocking(clientFD);

                    // 클라이언트 FD 셋팅
                    epoll_event clientEv;
                    clientEv.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLRDHUP;
                    clientEv.data.fd = clientFD;

                    epoll_ctl(m_workerEpollFDs[minCountIndex], EPOLL_CTL_ADD, clientFD, &clientEv);
                    m_workerClientCountMutexs[minCountIndex]->lock();
                    m_workerClientCount[minCountIndex]++;
                    m_workerClientCountMutexs[minCountIndex]->unlock();
                    
                    char ip[32] = {0,};
                    inet_ntop(AF_INET, (&clientAddr.sin_addr), ip, 31);
                    OnConnect(m_clients[clientFD], ip);
                }
            }
        }
    }

    // accept thread 종료 처리
    epoll_ctl(m_acceptEpollFD, EPOLL_CTL_DEL, m_listenFD, nullptr);
    close(m_listenFD);
    close(m_acceptEpollFD);

    cout << "AcceptThread 종료" << endl;
}

bool NetworkCore::CreateServerThread(int timeout, int eventSize)
{
    m_isWorkerRun = true;
    for(int i = 0; i < m_serverInfo.epollThreadCount; i++)
    {
        m_workerThreads.push_back(thread([this, timeout, eventSize, i]()
            {
                ServerThread(timeout, eventSize, i);
            })
        );
    }

    cout << "서버 시작!" << endl;
    return true;
}

void NetworkCore::ServerThread(int timeout, int eventSize, int index)
{
    epoll_event events[eventSize];

    while(m_isWorkerRun)
    {
        int n = epoll_wait(m_workerEpollFDs[index], events, eventSize, timeout);
        
        for(int i = 0; i < n; i++)
        {
            int clientFD = events[i].data.fd;

            // recv 할 것이 있다면
            if(events[i].events & EPOLLIN)
            {
                bool closed = false;
                int totalSize = 0;

                while(true)
                {
                    char buffer[m_serverInfo.recvBufSize];
                    memset(buffer, 0, sizeof(buffer));
                    ssize_t count = recv(clientFD, buffer, sizeof(buffer), 0);

                    if(count <= -1)
                    {
                        // 지금은 데이터를 읽을 상황이 아님. 혹은 읽을 것이 없음. 다음 EPOLLIN이 올 때까지 기다려
                        if(errno == EAGAIN || errno == EWOULDBLOCK)
                            break;
                        else
                        {
                            // 이건 진짜 에러
                            // 로그아웃 패킷을 보내서 저장시켜주고 죽일 것
                            closed = true;
                            break;
                        }
                    }
                    else if(count == 0)
                    {
                        closed = true;
                        break;
                    }
                    
                    totalSize += count;
                    m_clients[clientFD]->WriteRecvPacket(buffer, count);
                }

                if(closed)
                {
                    // epoll 관심사 제거
                    epoll_ctl(m_workerEpollFDs[index], EPOLL_CTL_DEL, clientFD, nullptr);

                    // 클라이언트 분배 현황 갱신
                    m_workerClientCountMutexs[index]->lock();
                    m_workerClientCount[index]--;
                    m_workerClientCountMutexs[index]->unlock();

                    // 클라이언트 FD 제거
                    close(clientFD);
                    m_clients[clientFD]->Disconnect();

                    OnClose(clientFD);

                    continue;
                }

                // 진짜 온 패킷
                char totalBuffer[totalSize];
                memset(totalBuffer, 0, sizeof(totalBuffer));

                m_clients[clientFD]->ReadRecvBuffer(totalBuffer, totalSize);
                OnReceive(clientFD, totalBuffer, totalSize);
            }

            // send 할 것이 있다면
            if(events[i].events & EPOLLOUT)
            {
                vector<char> sendData;

                // TODO(KBG): 송신 큐에서 가져오고 전부 send하기
                while(m_clients[clientFD]->PopSendPacket(sendData))
                {
                    ssize_t sendResult = send(clientFD, sendData.data(), sendData.size(), 0);
                    if(sendResult == -1 && errno == EAGAIN)     // 아직 보낼 수 있는 상황이 아님. 다음 EPOLLOUT까지 기다려
                        break;
                }

                // EPOLLOUT 제거하여 성능저하 제거
                DisableWrite(clientFD, index);
            }
        }
    }

    close(m_workerEpollFDs[index]);

    cout << "메인 서버 루프(" << index << ") 종료" << endl;
}

void NetworkCore::SetEpollFDs()
{
    // accept epoll
    m_acceptEpollFD = epoll_create1(EPOLL_CLOEXEC);
    if(m_acceptEpollFD == -1)
    {
        cout << "Accept Epoll FD를 만들 수 없습니다." << endl;
        return;
    }

    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLRDHUP;
    ev.data.fd = m_listenFD;
    epoll_ctl(m_acceptEpollFD, EPOLL_CTL_ADD, m_listenFD, &ev);

    // worker epoll
    for(int i = 0; i < m_serverInfo.epollThreadCount; i++)
    {
        int workerEpollFD = epoll_create1(EPOLL_CLOEXEC);
        if(workerEpollFD == -1)
        {
            cout << "epoll 소켓을 만들 수 없습니다." << endl;
            return;
        }

        m_workerEpollFDs.push_back(workerEpollFD);
        m_workerClientCount.push_back(0);
        m_workerClientCountMutexs.push_back(make_unique<mutex>());
    }
}

void NetworkCore::InitClientInfo()
{
    shared_ptr<NetworkCore> core = shared_from_this();

    m_clients.reserve(m_serverInfo.maxClientSize);
    for(int i = 0; i < m_serverInfo.maxClientSize; i++)
        m_clients.push_back(make_shared<Client>(i, m_serverInfo.recvBufSize * 2, core));
}

bool NetworkCore::ConnectClientInfo(int clientFD, int epollFDIndex)
{
    if(clientFD < 0 || clientFD >= m_serverInfo.maxClientSize)
        return false;
        
    return m_clients[clientFD]->Connect(epollFDIndex);
}

void NetworkCore::EnableWrite(int clientFD, int epollFDIndex)
{
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP;
    ev.data.fd = clientFD;

    epoll_ctl(m_workerEpollFDs[epollFDIndex], EPOLL_CTL_MOD, clientFD, &ev);
}

void NetworkCore::DisableWrite(int clientFD, int epollFDIndex)
{
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLRDHUP;
    ev.data.fd = clientFD;

    epoll_ctl(m_workerEpollFDs[epollFDIndex], EPOLL_CTL_MOD, clientFD, &ev);
}