#pragma once

// Define NOMINMAX to prevent min/max macro conflicts
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

// Include Windows headers first
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

using namespace std;

inline void DoWriteDevyatkina()
{
	std::wcout << std::endl;
}

template <class T, typename... Args> inline void DoWriteDevyatkina(T& value, Args... args)
{
	std::wcout << value << L" ";
	DoWriteDevyatkina(args...);
}

inline mutex g_WriteMutexDevyatkina;
template <typename... Args> inline void SafeWriteDevyatkina(Args... args)
{
	lock_guard<mutex> lg(g_WriteMutexDevyatkina);
	DoWriteDevyatkina(args...);
}

#pragma warning(disable : 4302 4311 4312 6031)

