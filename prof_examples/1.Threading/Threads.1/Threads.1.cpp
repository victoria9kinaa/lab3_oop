#include <windows.h>
#include <conio.h>
#include <iostream>
using namespace std;

#pragma warning(disable : 4302 4311 4312)

HANDLE hEvents[10];

HANDLE hEvent;
HANDLE hMutex;

CRITICAL_SECTION cs;

DWORD WINAPI MyThread(LPVOID lpParam)
{
	int i = (int)lpParam;
	//EnterCriticalSection(&cs);
	WaitForSingleObject(hMutex, INFINITE);
	std::cout << i << endl;
	//LeaveCriticalSection(&cs);
	ReleaseMutex(hMutex);


	WaitForSingleObject(hEvents[i], INFINITE);
//	Sleep(5000);
//	WaitForSingleObject(hEvent, INFINITE);

	std::cout << i << " done" << endl;
	CloseHandle(hEvents[i]);

	return 0;
}

int main()
{
	InitializeCriticalSection(&cs);
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);	// с автосбросом
//	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	// с ручным сбросом
	hMutex = CreateMutex(NULL, FALSE, NULL);
	
	int i;
	for (i = 0; i < 10; ++i)
	{
		hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		CreateThread(NULL, 0, MyThread, (LPVOID)i, 0, NULL);
	}

	for (int j = 0; j < 10; ++j)
	{
		_getch();
		SetEvent(hEvents[j]);
//		SetEvent(hEvent);
// 		PulseEvent(hEvent);
	}

	_getch();
	DeleteCriticalSection(&cs);
	CloseHandle(hEvent);
	CloseHandle(hMutex);

	return 0;
}
