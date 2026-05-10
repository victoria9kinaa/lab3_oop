#include "Session_Devyatkina.h"

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

Session_Devyatkina::Session_Devyatkina(int id, std::shared_ptr<ITransport_Devyatkina> transport)
    : m_id(id)
    , m_hThread(nullptr)
    , m_hEvent(nullptr)
    , m_done(false)
    , m_transport(std::move(transport))
{
    InitializeCriticalSection(&m_cs);

    // 🔥 ВАЖНО: ManualReset = TRUE (как в лекции)
    m_hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
}

Session_Devyatkina::~Session_Devyatkina()
{
    if (m_hThread && !m_done)
    {
        PostMessage({ MessageType_Devyatkina::Close, L"" });
        WaitForSingleObject(m_hThread, 3000);
    }

    if (m_hThread) { CloseHandle(m_hThread); m_hThread = nullptr; }
    if (m_hEvent) { CloseHandle(m_hEvent);  m_hEvent = nullptr; }

    DeleteCriticalSection(&m_cs);
}

HANDLE Session_Devyatkina::Start()
{
    m_hThread = CreateThread(nullptr, 0, ThreadProc, this, 0, nullptr);

    if (!m_hThread)
        ConPrint(L"[Session %d] CreateThread failed: %lu\n", m_id, GetLastError());
    else
        ConPrint(L"[Session %d] Thread created.\n", m_id);

    return m_hThread;
}

void Session_Devyatkina::send(const Message_Devyatkina& msg)
{
    Message_Devyatkina copy = msg;
    m_transport->send(*this, copy);
}

void Session_Devyatkina::receive(Message_Devyatkina& msg)
{
    m_transport->receive(*this, msg);
}

void Session_Devyatkina::PostMessage(const Message_Devyatkina& msg)
{
    send(msg);
}

void Session_Devyatkina::EnqueueMessage(const Message_Devyatkina& msg)
{
    EnterCriticalSection(&m_cs);
    m_queue.push(msg);
    SetEvent(m_hEvent);
    LeaveCriticalSection(&m_cs);
}

void Session_Devyatkina::DequeueMessage(Message_Devyatkina& msg)
{
    WaitForSingleObject(m_hEvent, INFINITE);
    while (true)
    {
        EnterCriticalSection(&m_cs);
        if (!m_queue.empty())
        {
            msg = m_queue.front();
            m_queue.pop();
            if (m_queue.empty())
                ResetEvent(m_hEvent);
            LeaveCriticalSection(&m_cs);
            return;
        }
        ResetEvent(m_hEvent);
        LeaveCriticalSection(&m_cs);
        WaitForSingleObject(m_hEvent, INFINITE);
    }
}

DWORD WINAPI Session_Devyatkina::ThreadProc(LPVOID param)
{
    static_cast<Session_Devyatkina*>(param)->Run();
    return 0;
}

void Session_Devyatkina::Run()
{
    ConPrint(L"[Session %d] Thread running...\n", m_id);

    while (true)
    {
        bool shouldExit = false;

        while (true)
        {
            Message_Devyatkina msg;
            receive(msg);
            switch (msg.type)
            {
            case MessageType_Devyatkina::Close:
                ConPrint(L"[Session %d] CLOSE received. Thread exiting.\n", m_id);
                shouldExit = true;
                break;
            case MessageType_Devyatkina::SendText:
                ConPrint(L"[Session %d] Message: %ls\n", m_id, msg.payload.c_str());
                break;
            default:
                break;
            }
            if (shouldExit) break;
            EnterCriticalSection(&m_cs);
            const bool queueEmpty = m_queue.empty();
            LeaveCriticalSection(&m_cs);
            if (queueEmpty) break;
        }
        if (shouldExit) break;
    }        

    m_done = true;
    ConPrint(L"[Session %d] Thread finished.\n", m_id);
}