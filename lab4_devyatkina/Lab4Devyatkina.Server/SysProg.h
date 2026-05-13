#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

using namespace std;

inline void DoWrite()
{
	std::wcout << std::endl;
}

template <class T, typename... Args> inline void DoWrite(T& value, Args... args)
{
	std::wcout << value << L" ";
	DoWrite(args...);
}

inline mutex m;
template <typename... Args> inline void SafeWrite(Args... args)
{
	lock_guard<mutex> lg(m);
	DoWrite(args...);
}

#pragma warning(disable : 4302 4311 4312 6031)
