#pragma once

#include <vector>
#include <algorithm>
#include <mutex>

using namespace std;

class RingBuffer
{
public:
    RingBuffer(int bufferSize);

    void Init();
    bool Write(char* buffer, int bufferSize);
    bool Read(char* recvBuffer, int readSize);

private:
    vector<char> m_buffer;
    int m_readPos;
    int m_writePos;
};