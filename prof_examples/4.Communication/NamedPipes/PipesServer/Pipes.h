#pragma once

#include <string>
#include <vector>
using namespace std;

inline int receiveInt(HANDLE hPipe)
{
	DWORD dwDone;
	int n;

	ReadFile(hPipe, &n, sizeof(n), &dwDone, NULL);
	return n;
}

inline void sendInt(HANDLE hPipe, int n, bool bFlush = true)
{
	DWORD dwDone;
	WriteFile(hPipe, &n, sizeof(n), &dwDone, NULL);
	if(bFlush)
		FlushFileBuffers(hPipe);
}

inline wstring receiveString(HANDLE hPipe)
{
	DWORD dwDone;
	int nLength = receiveInt(hPipe);

	wstring str;
	str.resize(nLength / sizeof(wchar_t));

	ReadFile(hPipe, str.data(), nLength, &dwDone, NULL);
	return str;
}

inline void sendString(HANDLE hPipe, const wstring& s)
{
	DWORD dwDone;
	int nLength = int(s.length() * sizeof(wchar_t));

	sendInt(hPipe, nLength, false);
	WriteFile(hPipe, s.data(), nLength, &dwDone, NULL);
	FlushFileBuffers(hPipe);
}

