#include "UserManager.h"

void UserManager::Init(const int maxClientSize, const int threadSize)
{
    m_users.reserve(maxClientSize);
    for(int i = 0; i < maxClientSize; i++)
        m_users.emplace_back(make_shared<User>());
}

shared_ptr<User> UserManager::GetUser(const int clientIndex)
{
    if(clientIndex < 0 || clientIndex >= m_users.size())
        return nullptr;

    return m_users[clientIndex];
}