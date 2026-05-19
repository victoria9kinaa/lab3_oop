#include "SessionDevyatkina.h"

SessionDevyatkina::SessionDevyatkina(int sessionID)
    : sessionID(sessionID), m_lastRequest(std::chrono::steady_clock::now())
{
}

void SessionDevyatkina::addMessage(MessageDevyatkina& m)
{
    std::lock_guard lg(m_mx);
    m_messages.push(m);
    m_ev.notify_one();
}

void SessionDevyatkina::addMessage(int type, const std::wstring& data, int to, int from)
{
    MessageDevyatkina m(type, data, to, from);
    addMessage(m);
}

bool SessionDevyatkina::getMessage(MessageDevyatkina& m)
{
    std::unique_lock ul(m_mx);
    // ИСПРАВЛЕНО: ждём сообщение ИЛИ сигнал остановки
    m_ev.wait(ul, [&] { return !m_messages.empty() || m_stopped.load(); });

    if (m_stopped && m_messages.empty())
        return false;  // воркер выйдет из цикла

    m = m_messages.front();
    m_messages.pop();
    return true;
}

// ДОБАВЛЕНО: будит поток из wait() при удалении сессии
void SessionDevyatkina::stop()
{
    std::lock_guard lg(m_mx);
    m_stopped = true;
    m_ev.notify_all();
}

void SessionDevyatkina::updateActivity()
{
    std::lock_guard lg(m_mx);
    m_lastRequest = std::chrono::steady_clock::now();
}

bool SessionDevyatkina::isTimedOut(std::chrono::milliseconds timeout) const
{
    return (std::chrono::steady_clock::now() - m_lastRequest) > timeout;
}
