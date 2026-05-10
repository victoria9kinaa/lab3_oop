// SharedConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include "../SharedDLL/SharedDLL.h"

int main()
{
	std::locale::global(std::locale("rus_rus.866"));
	std::wcin.imbue(std::locale());
	std::wcout.imbue(std::locale());

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	wchar_t path[] = L"SharedSharp.exe";
	CreateProcess(NULL, path, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	while (true)
	{
		std::wcin >> gsSharedString;
		if (ghWnd)
		{
			SetForegroundWindow(ghWnd);
		}
	}
}
