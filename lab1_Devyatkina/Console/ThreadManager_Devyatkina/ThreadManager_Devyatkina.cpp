// ThreadManager_Devyatkina.cpp

#include "ThreadManager_Devyatkina.h"
#include <algorithm>

static void ConPrint(const wchar_t* fmt, ...)
{
    wchar_t buf[512];
    va_list args;
    va_start(args, fmt);
    vswprintf_s(buf, fmt, args);
    va_end(args);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written = 0;
    WriteConsoleW(hOut, buf, (DWORD)wcslen(buf), &written, nullptr);
}

ThreadManager_Devyatkina::ThreadManager_Devyatkina()
    : m_transport(std::make_shared<LocalTransport_Devyatkina>())
    , m_nextId(1)
{
}
ThreadManager_Devyatkina::~ThreadManager_Devyatkina() { StopAllSessions(); }

void ThreadManager_Devyatkina::CreateSessions(int count)
{
    Cleanup();
    for (int i = 0; i < count; ++i)
    {
        auto session = std::make_unique<Session_Devyatkina>(m_nextId++, m_transport);
        session->Start();
        m_sessions.push_back(std::move(session));
    }
    ConPrint(L"[Manager] Created %d session(s). Total active: %d\n",
             count, (int)m_sessions.size());
}

bool ThreadManager_Devyatkina::StopLastSession()
{
    Cleanup();
    if (m_sessions.empty()) return false;
    Session_Devyatkina* last = m_sessions.back().get();
    ConPrint(L"[Manager] Sending Close to Session %d\n", last->GetId());
    last->PostMessage({ MessageType_Devyatkina::Close, L"" });
    if (last->GetThreadHandle())
        WaitForSingleObject(last->GetThreadHandle(), 5000);
    m_sessions.pop_back();
    ConPrint(L"[Manager] Session stopped. Active: %d\n", (int)m_sessions.size());
    return true;
}

void ThreadManager_Devyatkina::StopAllSessions()
{
    for (auto& s : m_sessions)
        s->PostMessage({ MessageType_Devyatkina::Close, L"" });
    std::vector<HANDLE> handles;
    for (auto& s : m_sessions)
        if (s->GetThreadHandle())
            handles.push_back(s->GetThreadHandle());
    if (!handles.empty())
        WaitForMultipleObjects((DWORD)handles.size(), handles.data(), TRUE, 5000);
    m_sessions.clear();
    ConPrint(L"[Manager] All sessions stopped.\n");
}

void ThreadManager_Devyatkina::SendToLast(const std::wstring& text)
{
    Cleanup();
    if (m_sessions.empty()) { ConPrint(L"[Manager] No active sessions.\n"); return; }
    m_sessions.back()->PostMessage({ MessageType_Devyatkina::SendText, text });
}

int ThreadManager_Devyatkina::Count() const { return (int)m_sessions.size(); }

void ThreadManager_Devyatkina::Cleanup()
{
    m_sessions.erase(
        std::remove_if(m_sessions.begin(), m_sessions.end(),
            [](const std::unique_ptr<Session_Devyatkina>& s){ return s->IsDone(); }),
        m_sessions.end());
}
