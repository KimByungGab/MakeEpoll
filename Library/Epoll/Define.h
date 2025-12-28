#pragma once

#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

constexpr int MAX_BUFFER_SIZE = 8192;
constexpr short MAX_EVENT_SIZE = 100;       // epoll 이벤트 최대 사이즈

// 소켓 모드
enum class SOCKET_MODE : short
{
    NONE = 0,
    REUSE_ADDR = 1,     // 주소 재사용 모드
};

// 서버 시작할 때 소켓 정보
struct _SERVER_INFO
{
    SOCKET_MODE mode;
    int port;
    int recvBufSize;
    int eventSize;
    int maxSize;
    int timeout;
    int epollThreadCount;
    int workerThreadCount;
    int maxClientSize;

    _SERVER_INFO()
    {
        mode = SOCKET_MODE::NONE;
        port = -1;
        recvBufSize = MAX_BUFFER_SIZE;
        eventSize = MAX_EVENT_SIZE;
        maxSize = SOMAXCONN;
        timeout = -1;
        epollThreadCount = 1;
        workerThreadCount = 1;
        maxClientSize = 100;
    }
};