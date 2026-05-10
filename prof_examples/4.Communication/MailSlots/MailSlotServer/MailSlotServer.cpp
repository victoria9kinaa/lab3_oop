// MailSlotServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>

using namespace std;

void LaunchClient()
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	wchar_t path[] = L"MailSlotClient.exe";
	CreateProcess(NULL, path, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

int main()
{
	std::locale::global(std::locale("rus_rus.866"));
	wcin.imbue(std::locale());
	wcout.imbue(std::locale());

	//
	// \\.\mailslot\<slotname>
	//
	HANDLE hMail = CreateMailslot(L"\\\\.\\mailslot\\myslot", 0, MAILSLOT_WAIT_FOREVER, NULL);
	for (int i = 0; i < 3; ++i)
	{
		LaunchClient();
	}
	while (true)
	{
		DWORD msgSize,dwRead;
		GetMailslotInfo(hMail, NULL, &msgSize, NULL, NULL);
		if (msgSize == MAILSLOT_NO_MESSAGE)
		{
			Sleep(100);
			continue;
		}
		char* pBuff = new char[msgSize];
		ReadFile(hMail, pBuff, msgSize, &dwRead, NULL);
		wcout << (wchar_t*)pBuff << endl;

		if (!lstrcmp((wchar_t*)pBuff, L"quit"))
		{
			delete[] pBuff;
			break;
		}
		delete[] pBuff;
	}
	CloseHandle(hMail);
}
