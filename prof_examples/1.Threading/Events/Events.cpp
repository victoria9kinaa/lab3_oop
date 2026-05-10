#include "../Common/SysProg.API.h"

HANDLE hEvent;
HANDLE hEvents[100];

DWORD WINAPI MyThread(LPVOID lpParam)
{
	int i = (int)lpParam;
	SafeWrite(i);

	WaitForSingleObject(hEvents[i], INFINITE);

//	Sleep(10000);
//	WaitForSingleObject(hEvent, INFINITE);

	SafeWrite(i, "done");
	CloseHandle(hEvents[i]);

	return 0;
}

int main()
{
	//	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	int i;
	for (i = 0; i < 10; ++i)
	{
		hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		CreateThread(NULL, 0, MyThread, (LPVOID)i, 0, NULL);
	}


	HANDLE hStartEvent = CreateEvent(NULL, FALSE, FALSE, L"StartEvent");
	HANDLE hStopEvent = CreateEvent(NULL, FALSE, FALSE, L"StopEvent");
	HANDLE hConfirmEvent = CreateEvent(NULL, FALSE, FALSE, L"ConfirmEvent");
	HANDLE hControlEvents[2] = { hStartEvent, hStopEvent };
	while (i)
	{
		int n = WaitForMultipleObjects(2, hControlEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
		switch (n)
		{
			case 0:
				hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
				CreateThread(NULL, 0, MyThread, (LPVOID)i, 0, NULL);
				SetEvent(hConfirmEvent);
				i++;
				break;
			case 1:
				ResetEvent(hStopEvent);
				SetEvent(hEvents[--i]);
				SetEvent(hEvent);
//				PulseEvent(hEvent);
				SetEvent(hConfirmEvent);
				break;
		}
	}
	SetEvent(hConfirmEvent);

	return 0;
}
