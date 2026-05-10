// main.cpp  -  IPC main loop (Named Events, WinAPI)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <conio.h>

#include "ThreadManager_Devyatkina/ThreadManager_Devyatkina.h"

static const wchar_t* COUNT_PREFIX = L"Global\\Devyatkina_Count_";

static void ConPrint(const wchar_t* fmt, ...)
{
    wchar_t buf[1024];
    va_list args;
    va_start(args, fmt);
    vswprintf_s(buf, fmt, args);
    va_end(args);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written = 0;
    WriteConsoleW(hOut, buf, (DWORD)wcslen(buf), &written, nullptr);
}

static int ReadCountFromEvents()
{
    for (int n = 1; n <= 20; ++n)
    {
        wchar_t name[128];
        swprintf_s(name, L"%ls%d", COUNT_PREFIX, n);
        HANDLE h = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, name);
        if (h) { CloseHandle(h); return n; }
    }
    for (int n = 1; n <= 20; ++n)
    {
        wchar_t name[128];
        swprintf_s(name, L"Devyatkina_Count_%d", n);
        HANDLE h = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, name);
        if (h) { CloseHandle(h); return n; }
    }
    return 1;
}

static HANDLE TryOpenEvent(const wchar_t* globalName, const wchar_t* localName)
{
    HANDLE h = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, globalName);
    if (h) return h;
    return OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, localName);
}

int wmain()
{
    ConPrint(L"=== ConsoleApp Devyatkina started ===\n");

    // GUI -> Console signals
    HANDLE hStart = TryOpenEvent(L"Global\\Devyatkina_StartThreads", L"Devyatkina_StartThreads");
    HANDLE hStop  = TryOpenEvent(L"Global\\Devyatkina_StopThread",   L"Devyatkina_StopThread");
    HANDLE hSend  = TryOpenEvent(L"Global\\Devyatkina_SendMessage",  L"Devyatkina_SendMessage");
    // Отдельный сигнал завершения консоли (Kill)
    HANDLE hKill  = TryOpenEvent(L"Global\\Devyatkina_KillConsole",  L"Devyatkina_KillConsole");

    if (!hStart || !hStop || !hSend || !hKill)
    {
        ConPrint(L"[ERROR] Cannot open named events. Code: %lu\n", GetLastError());
        ConPrint(L"[ERROR] Start GUI first, then Console.\n");
        if (hStart) CloseHandle(hStart);
        if (hStop)  CloseHandle(hStop);
        if (hSend)  CloseHandle(hSend);
        if (hKill)  CloseHandle(hKill);
        ConPrint(L"Press any key to exit...\n");
        _getch();
        return 1;
    }

    // Console -> GUI confirmation signals
    HANDLE hCreated = TryOpenEvent(L"Global\\Devyatkina_ThreadsCreated", L"Devyatkina_ThreadsCreated");
    HANDLE hStopped = TryOpenEvent(L"Global\\Devyatkina_ThreadStopped",  L"Devyatkina_ThreadStopped");

    ConPrint(L"[Console] All events opened OK. Waiting for GUI commands...\n");

    ThreadManager_Devyatkina manager;

    // WaitForMultipleObjects: Start=0, Stop=1, Send=2, Kill=3
    HANDLE waitHandles[4] = { hStart, hStop, hSend, hKill };
    bool running = true;

    while (running)
    {
        ConPrint(L"[Console] Ready. Waiting for signal...\n");
        DWORD result = WaitForMultipleObjects(4, waitHandles, FALSE, INFINITE);

        switch (result)
        {
        // START: create N threads
        case WAIT_OBJECT_0 + 0:
        {
            int n = ReadCountFromEvents();
            ConPrint(L"\n[Console] >>> START: creating %d thread(s)\n", n);
            manager.CreateSessions(n);
            if (hCreated) SetEvent(hCreated);
            ConPrint(L"[Console] Confirmation sent (ThreadsCreated). Active: %d\n", manager.Count());
            break;
        }
        // STOP: stop last thread
        case WAIT_OBJECT_0 + 1:
        {
            ConPrint(L"\n[Console] >>> STOP: stopping last thread\n");
            manager.StopLastSession();
            if (hStopped) SetEvent(hStopped);
            ConPrint(L"[Console] Confirmation sent (ThreadStopped). Active: %d\n", manager.Count());
            break;
        }
        // SEND: send message to last session
        case WAIT_OBJECT_0 + 2:
        {
            ConPrint(L"\n[Console] >>> SEND signal\n");
            manager.SendToLast(L"<message from GUI>");
            break;
        }
        // KILL: terminate console process
        case WAIT_OBJECT_0 + 3:
        {
            ConPrint(L"\n[Console] >>> KILL signal received. Shutting down.\n");
            running = false;
            break;
        }
        default:
            ConPrint(L"[Console] WaitForMultipleObjects error: %lu\n", GetLastError());
            running = false;
            break;
        }
    }

    ConPrint(L"\n[Console] Stopping all sessions...\n");
    manager.StopAllSessions();

    if (hStart)   CloseHandle(hStart);
    if (hStop)    CloseHandle(hStop);
    if (hSend)    CloseHandle(hSend);
    if (hKill)    CloseHandle(hKill);
    if (hCreated) CloseHandle(hCreated);
    if (hStopped) CloseHandle(hStopped);

    ConPrint(L"=== ConsoleApp Devyatkina finished ===\n");
    return 0;
}
