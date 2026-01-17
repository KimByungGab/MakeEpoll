#pragma once

#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include <memory>

#include <cstring>

#include "Define.h"
#include "Client.h"

using namespace std;

class NetworkCore : public enable_shared_from_this<NetworkCore>
{
public:
    NetworkCore(_SERVER_INFO socketInfo);

    virtual void OnConnect(shared_ptr<Client> client, string ip) = 0;
    virtual void OnClose(const int clientIndex) = 0;
    virtual void OnReceive(const int clientIndex, char* buffer, int bufferSize) = 0;

    /**
     * @brief 서버 시작
     * @author 김병갑
     * @param void
     */
    void StartServer();

    /**
     * @brief 서버 종료
     * @author 김병갑
     * @param void
     */
    void EndServer();

    /**
     * @brief 쓰기 이벤트 활성화
     * @author 김병갑
     * @param clientFD 클라이언트 파일 디스크립터
     * @param epollFDIndex Epoll FD 인덱스
     */
    void EnableWrite(int clientFD, int epollFDIndex);

private:
    /**
     * @brief 초기화
     * @author 김병갑
     * @param void
     */
    void Init();

    /**
     * @brief 소켓 생성
     * @author 김병갑
     * @param void
     * @return 성공 혹은 실패
     */
    bool CreateSocket();

    /**
     * @brief 소켓 바인드
     * @author 김병갑
     * @param void
     * @return 성공 혹은 실패
     */
    bool BindSocket();

    /**
     * @brief 소켓 리슨
     * @author 김병갑
     * @param void
     * @return 성공 혹은 실패
     */
    bool ListenSocket();

    /**
     * @brief 주소 재사용 가능한 모드
     * @author 김병갑
     * @param FD 파일 디스크립터
     */
    void SetReuseAddr(int& FD);

    /**
     * @brief 논블로킹 모드로 소켓 세팅
     * @author 김병갑
     * @param FD 파일 디스크립터
     */
    void SetNonBlocking(int& FD);

    /**
     * @brief Epoll 시작
     * @author 김병갑
     * @param void
     */
    void StartEpoll();

    /**
     * @brief Accept 쓰레드 생성
     * @author 김병갑
     * @param timeout 만료시간
     * @param eventSize Epoll 이벤트 사이즈
     * @return 성공 혹은 실패
     */
    bool CreateAcceptThread(int timeout, int eventSize);

    /**
     * @brief Accept 쓰레드
     * @author 김병갑
     * @param timeout 만료시간
     * @param eventSize Epoll 이벤트 사이즈
     */
    void AcceptThread(int timeout, int eventSize);

    /**
     * @brief 메인 서버 쓰레드 생성
     * @author 김병갑
     * @param timeout 만료시간
     * @param eventSize Epoll 이벤트 사이즈
     * @return 성공 혹은 실패
     */
    bool CreateServerThread(int timeout, int eventSize);

    /**
     * @brief 메인 서버 쓰레드
     * @author 김병갑
     * @param timeout 만료시간
     * @param eventSize Epoll 이벤트 사이즈
     * @param index 사용할 FD 인덱스
     */
    void ServerThread(int timeout, int eventSize, int index);

    /**
     * @brief epoll FD들 셋팅
     * @author 김병갑
     * @param void
     */
    void SetEpollFDs();

    /**
     * @brief 클라이언트 정보 초기화
     * @author 김병갑
     * @param void
     */
    void InitClientInfo();

    /**
     * @brief 클라이언트 정보 연결
     * @author 김병갑
     * @param clientFD 클라이언트 파일 디스크립터
     * @return 성공 혹은 실패
     */
    bool ConnectClientInfo(int clientFD, int epollFDIndex);
    /**
     * @brief 쓰기 이벤트 비활성화
     * @author 김병갑
     * @param clientFD 클라이언트 파일 디스크립터
     * @param epollFDIndex Epoll FD 인덱스
     */
    void DisableWrite(int clientFD, int epollFDIndex);
    
public:
    _SERVER_INFO m_serverInfo;                      // 서버를 시작하기 위해 받은 정보

private:
    int m_listenFD;                                 // listen용 FD

    int m_acceptEpollFD;                            // Accept용 Epoll FD
    bool m_isAcceptRun;                             // Accept onoff 스위치
    thread m_acceptThread;

    vector<int> m_workerEpollFDs;
    bool m_isWorkerRun;
    vector<thread> m_workerThreads;
    vector<short> m_workerClientCount;                          // 할당된 클라이언트 갯수
    vector<unique_ptr<mutex>> m_workerClientCountMutexs;        // 할당된 클라이언트 갯수 mutex.
    // vector<mutex>는 OS 리소스를 관리하는 특수개체라 이동 및 복사가 불가능. 때문에 unique_ptr을 만들어 unique_lock을 이용한다.

    vector<shared_ptr<Client>> m_clients;                       // 클라이언트
};