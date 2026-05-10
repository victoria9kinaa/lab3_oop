// PipesServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <thread>
#include "Pipes.h"

void LaunchClient()
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	wchar_t path[] = L"PipesClient.exe";
	CreateProcess(NULL, path, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

void ProcessClient(HANDLE hPipe)
{
	static int i = 0;
	while (true)
	{
		wstring s = receiveString(hPipe);
		wcout << s << endl;
		if (s == L"quit")
		{
			break;
		}
		sendInt(hPipe, ++i);
	}
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
}

int main()
{
	std::locale::global(std::locale("rus_rus.866"));
	wcin.imbue(std::locale());
	wcout.imbue(std::locale());

	for (int i = 0; i < 3; ++i)
	{
		LaunchClient();
	}

	while (true)
	{
		HANDLE hPipe = CreateNamedPipe(L"\\\\.\\pipe\\MyPipe", 
			PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 
			1024, 1024, 0, NULL);
		ConnectNamedPipe(hPipe, NULL);
		ProcessClient(hPipe);
//		thread(ProcessClient, hPipe).detach();
	}
}
