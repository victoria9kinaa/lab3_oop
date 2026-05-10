#include "WorkerSessionDevyatkina.h"

#include <fstream>
#include <Windows.h>

static std::string ToUtf8Devyatkina(const std::wstring& text)
{
    if (text.empty()) return {};

    const int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
        nullptr, 0, nullptr, nullptr);
    if (size <= 0) return {};

    std::string utf8(static_cast<size_t>(size), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
        utf8.data(), size, nullptr, nullptr);
    return utf8;
}

WorkerSessionDevyatkina::WorkerSessionDevyatkina(int id)
    : m_id(id)
    , m_stopping(false)
{
}

WorkerSessionDevyatkina::~WorkerSessionDevyatkina()
{
    stop();
}

void WorkerSessionDevyatkina::start()
{
    m_thread = std::thread(&WorkerSessionDevyatkina::run, this);
}

void WorkerSessionDevyatkina::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stopping = true;
    }
    m_cv.notify_all();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void WorkerSessionDevyatkina::post(const std::wstring& text)
{
    ThreadMessageDevyatkina msg;
    msg.payload = text;
    send(msg);
}

void WorkerSessionDevyatkina::send(const ThreadMessageDevyatkina& msg)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(msg);
    }
    m_cv.notify_one();
}

bool WorkerSessionDevyatkina::receive(ThreadMessageDevyatkina& msg)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this]() { return m_stopping || !m_queue.empty(); });
    if (m_stopping && m_queue.empty()) return false;

    msg = m_queue.front();
    m_queue.pop();
    return true;
}

int WorkerSessionDevyatkina::id() const
{
    return m_id;
}

void WorkerSessionDevyatkina::run()
{
    const std::wstring fileName = std::to_wstring(m_id) + L".txt";
    std::ofstream out(fileName.c_str(), std::ios::app | std::ios::binary);

    while (true)
    {
        ThreadMessageDevyatkina msg;
        if (!receive(msg)) break;

        const std::string utf8 = ToUtf8Devyatkina(msg.payload);
        out << utf8 << "\r\n";
        out.flush();
    }
}
