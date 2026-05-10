// PipesClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <conio.h>
#include "../PipesServer/Pipes.h"

using namespace std;

int main()
{
	std::locale::global(std::locale("rus_rus.866"));
	wcin.imbue(std::locale());
	wcout.imbue(std::locale());

	if (WaitNamedPipe(L"\\\\.\\pipe\\MyPipe", 3000))
	{
		HANDLE hPipe = CreateFile(L"\\\\.\\pipe\\MyPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		while (true)
		{
			wstring s;
			wcin >> s;
			sendString(hPipe, s);
			if (s == L"quit")
			{
				break;
			}
			wcout << receiveInt(hPipe) << endl;
		}
		CloseHandle(hPipe);
	}
	else
	{
		wcout << L"No pipes available" << endl;
		_getch();
	}
}

