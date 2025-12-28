#pragma once

#include <vector>
#include <string>
#include <cstring>

using namespace std;

#define ADD_RESERVE_SIZE 1024

class DataArchive
{	
public:
    DataArchive()
    {
        Init();
        buffer.reserve(ADD_RESERVE_SIZE);
    }
    
    inline void Init()
    {
        readPos = 0;
        writePos = 0;
        buffer.clear();
    }

    template<typename T>
    inline DataArchive& operator<<(const T& data)
    {
        AddBuffer(reinterpret_cast<const char*>(&data), sizeof(T));
    
        return *this;
    }

    template<typename T>
    inline DataArchive& operator>>(T& data)
    {
        if(readPos + sizeof(T) > writePos)
            return *this;
    
        memcpy(&data, &buffer[readPos], sizeof(T));
        readPos += sizeof(T);
    
        return *this;
    }

    inline DataArchive& operator<<(const string& data)
    {
        short length = data.length();
        *this << length;
    
        AddBuffer(data.c_str(), length * sizeof(char));
    
        return *this;
    }

    inline DataArchive& operator>>(string& data)
    {
        short length;
        *this >> length;
    
        int dataSize = length * sizeof(char);
        if(readPos + dataSize > writePos)
            return *this;
    
        data.assign(reinterpret_cast<char*>(&buffer[readPos]), dataSize);
        readPos += dataSize;
    
        return *this;
    }

    inline DataArchive& operator<<(const wstring& data)
    {
        short length = data.length();
        *this << length;

        AddBuffer(reinterpret_cast<const char*>(data.c_str()), length * sizeof(wchar_t));

        return *this;
    }

    inline DataArchive& operator>>(wstring& data)
    {
        short length;
        *this >> length;

        int dataSize = length * sizeof(wchar_t);
        if (readPos + dataSize > writePos)
            return *this;

        data.assign(reinterpret_cast<wchar_t*>(&buffer[readPos]), dataSize);
        readPos += dataSize;
        return *this;
    }
    
    inline DataArchive& operator<<(DataArchive& DA)
    {
        AddBuffer(const_cast<const char*>(DA.GetBufferToPtr()), DA.GetBufferSize());
    
        return *this;
    }
    
    inline void AddBuffer(const char* data, int dataSize)
    {
        CheckSize(dataSize);
        buffer.insert(buffer.end(), data, data + dataSize);
        writePos += dataSize;
    }
    
    inline void SetBuffer(const char* data, int dataSize = 1024)
    {
        Init();
        AddBuffer(data, dataSize);
    }
    
    inline char* GetBufferToPtr() { return &buffer[0]; }
    inline int GetBufferSize() { return buffer.size(); }
    
private:
    inline void CheckSize(int& dataSize)
    {
        if(writePos + dataSize <= buffer.capacity())
            return;
        
        buffer.reserve(buffer.capacity() * 2);
    }

private:
    size_t readPos;
    size_t writePos;
    vector<char> buffer;
};