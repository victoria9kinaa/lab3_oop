#pragma once

#ifndef _WIN32_WINNT
#define	_WIN32_WINNT	0x0A00
#endif						

#include <boost/asio.hpp>
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

using boost::asio::ip::tcp;
using namespace std;

template <class T>
inline void sendData(tcp::socket& s, T* data, size_t n = 0)
{
	if (!n)
		n = sizeof(T);
	boost::system::error_code error;
	boost::asio::write(s, boost::asio::buffer(data, n), error);
	if (error)
		throw boost::system::system_error(error);
}

template <class T>
inline void receiveData(tcp::socket& s, T* data, size_t n = 0)
{
	if (!n)
		n = sizeof(T);
	boost::system::error_code error;
	boost::asio::read(s, boost::asio::buffer(data, n), error);
	if (error)
		throw boost::system::system_error(error);
}

inline void doWrite()
{
	std::wcout << std::endl;
}

template <class T, typename... Args> inline void doWrite(T& value, Args... args)
{
	std::wcout << value << L" ";
	doWrite(args...);
}

mutex m;
template <typename... Args> inline void safeWrite(Args... args)
{
	lock_guard<mutex> lg(m);
	doWrite(args...);
}

void launchClient(wstring path)
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	CreateProcess(NULL, path.data(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

#pragma warning(disable : 4302 4311 4312 6031)
