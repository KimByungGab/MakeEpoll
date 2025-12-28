#pragma once

template <typename T>
class Singleton
{
public:
    static T& getInstance()
    {
        static T instance;
        return instance;    // 정적 지역 변수는 main함수가 끝나며 소멸처리됨. C++11부터는 Thread-safe함
    }

    // 상속받은 클래스에만 접근이 가능해야되므로 복사 및 대입은 삭제.
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:  // 상속 전용 싱글톤이기 때문에 상속 받은 클래스만 접근가능.
    Singleton() = default;
    virtual ~Singleton() = default;
};