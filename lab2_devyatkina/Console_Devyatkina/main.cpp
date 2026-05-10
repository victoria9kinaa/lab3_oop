#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <atomic>
#include <string>
#include <thread>
#include <cwchar>

#include "ThreadManagerDevyatkina.h"
#include "../MMFLib_Devyatkina/ExportApiDevyatkina.h"
#include "../MMFLib_Devyatkina/MessageDevyatkina.h"

namespace
{
    constexpr int COMMAND_TIMEOUT_MS = 250;
}

static void ConPrintDevyatkina(const wchar_t* fmt, ...)
{
    wchar_t buffer[4096];
    va_list args;
    va_start(args, fmt);
    vswprintf_s(buffer, fmt, args);
    va_end(args);

    DWORD written = 0;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buffer, static_cast<DWORD>(wcslen(buffer)), &written, nullptr);
}

int wmain()
{
    ConPrintDevyatkina(L"[Console] ConsoleAppDevyatkina started.\n");
    ThreadManagerDevyatkina manager;
    std::atomic<bool> running { true };
    while (running)
    {
        if (!WaitCommandDevyatkina(COMMAND_TIMEOUT_MS))
        {
            continue;
        }

        int commandType = static_cast<int>(MessageTypeDevyatkina::None);
        int targetId = 0;
        int param = 0;
        wchar_t textBuffer[8192];
        if (!ReceiveCommandDevyatkina(&commandType, &targetId, &param, textBuffer, static_cast<int>(_countof(textBuffer))))
        {
            SendAckDevyatkina(0, L"Console: failed to read command from MMF.");
            continue;
        }

        const std::wstring text(textBuffer);
        const auto type = static_cast<MessageTypeDevyatkina>(commandType);
        switch (type)
        {
            case MessageTypeDevyatkina::StartThreads:
                manager.create(param > 0 ? param : 1);
                ConPrintDevyatkina(L"[Console] Created %d thread(s). Active: %d\n", param > 0 ? param : 1, manager.count());
                SendAckDevyatkina(1, L"Threads created.");
                break;
            case MessageTypeDevyatkina::StopLastThread:
                manager.stopLast();
                ConPrintDevyatkina(L"[Console] Stopped last thread. Active: %d\n", manager.count());
                SendAckDevyatkina(1, L"Last thread stopped.");
                break;
            case MessageTypeDevyatkina::SendText:
                ConPrintDevyatkina(L"[Main] %ls\n", text.c_str());
                if (targetId == -1)
                {
                    manager.sendToAll(text);
                    SendAckDevyatkina(1, L"Text delivered to all threads.");
                }
                else if (targetId > 0)
                {
                    manager.sendTo(targetId, text);
                    SendAckDevyatkina(1, L"Text delivered to selected thread.");
                }
                else
                {
                    SendAckDevyatkina(1, L"Text delivered to main thread.");
                }
                break;
            case MessageTypeDevyatkina::Shutdown:
                running = false;
                SendAckDevyatkina(1, L"Console shutdown requested.");
                break;
            default:
                SendAckDevyatkina(0, L"Unknown command type.");
                break;
        }
    }

    manager.stopAll();
    ConPrintDevyatkina(L"[Console] All worker threads stopped.\n");
    ConPrintDevyatkina(L"[Console] ConsoleAppDevyatkina finished.\n");
    return 0;
}
