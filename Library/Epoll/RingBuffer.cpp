#include "RingBuffer.h"

RingBuffer::RingBuffer(int bufferSize)
{
    Init();
    m_buffer.resize(bufferSize);
}

void RingBuffer::Init()
{
    m_readPos = 0;
    m_writePos = 0;
    m_buffer.clear();
}

bool RingBuffer::Write(char* buffer, int bufferSize)
{
    // 버퍼 사이즈가 0은 말도 안 됨, 버퍼 사이즈를 초과한다면 readPos를 침범하게 됨
    if (bufferSize <= 0 || bufferSize > m_buffer.size())
        return false;

    // 버퍼 크기를 넘지 않는다면
    if (m_writePos + bufferSize <= m_buffer.size())
    {
        // copy
        copy(buffer, buffer + bufferSize, m_buffer.data() + m_writePos);
        m_writePos += bufferSize;
    }
    else    // 버퍼 크기를 만약 넘는다면?
    {
        // 끝까지 얼마 남았는지 사이즈를 계산 후 넣기
        int firstSize = m_buffer.size() - m_writePos;

        // 만약 남아있는 크기가 readPos를 침범한다면
        if (bufferSize - firstSize >= m_readPos)
            return false;

        // copy
        copy(buffer, buffer + firstSize, m_buffer.data() + m_writePos);
        copy(buffer + firstSize, buffer + bufferSize, m_buffer.data());
        m_writePos = bufferSize - firstSize;
    }

    return true;
}

bool RingBuffer::Read(char* recvBuffer, int readSize)
{
    // 읽을 것이 없는데 뭘 읽으라는거임
    if (m_readPos == m_writePos)
        return false;

    // 버퍼가 정상적으로 쌓이는 구조라면
    if (m_readPos < m_writePos)
    {
        copy(m_buffer.data() + m_readPos, m_buffer.data() + m_readPos + readSize, recvBuffer);
        m_readPos += readSize;
    }
    else    // writePos가 처음으로 돌아갔다면
    {
        // 읽을 사이즈가 많이 남아있다면 처음으로 돌아가지 않아도 됨
        if (readSize <= m_buffer.size() - m_readPos)
        {
            copy(m_buffer.data() + m_readPos, m_buffer.data() + m_readPos + readSize, recvBuffer);
            m_readPos += readSize;
        }
        else    // 나머지 읽고 처음으로 돌아가서 또 읽어야한다면
        {
            int firstSize = m_buffer.size() - m_readPos;
            copy(m_buffer.data() + m_readPos, m_buffer.data() + m_buffer.size(), recvBuffer);
            copy(m_buffer.data(), m_buffer.data() + readSize - firstSize, recvBuffer + firstSize);
            m_readPos = readSize - firstSize;
        }
    }

    return true;
}