#include "ThreadManagerDevyatkina.h"

#include <algorithm>

ThreadManagerDevyatkina::ThreadManagerDevyatkina()
    : m_nextId(1)
{
}

ThreadManagerDevyatkina::~ThreadManagerDevyatkina()
{
    stopAll();
}

void ThreadManagerDevyatkina::create(int count)
{
    for (int i = 0; i < count; ++i)
    {
        auto s = std::make_unique<WorkerSessionDevyatkina>(m_nextId++);
        s->start();
        m_sessions.push_back(std::move(s));
    }
}

void ThreadManagerDevyatkina::stopLast()
{
    if (m_sessions.empty()) return;
    m_sessions.back()->stop();
    m_sessions.pop_back();
}

void ThreadManagerDevyatkina::stopAll()
{
    for (auto& session : m_sessions)
    {
        session->stop();
    }
    m_sessions.clear();
}

int ThreadManagerDevyatkina::count() const
{
    return static_cast<int>(m_sessions.size());
}

void ThreadManagerDevyatkina::sendTo(int targetId, const std::wstring& text)
{
    for (const auto& session : m_sessions)
    {
        if (session->id() == targetId)
        {
            session->post(text);
            break;
        }
    }
}

void ThreadManagerDevyatkina::sendToAll(const std::wstring& text)
{
    for (const auto& session : m_sessions)
    {
        session->post(text);
    }
}
