#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

#include "Object/User.h"

using namespace std;

class UserManager
{
public:
    void Init(const int maxClientSize, const int threadSize = 1);
    shared_ptr<User> GetUser(const int clientIndex);
    int GetMaxUserSize() { return m_users.size(); }

private:
    vector<shared_ptr<User>> m_users;
};