#include "../Common/SysProg.API.h"

HANDLE hSemaphore;

HANDLE hTimer;

void MyThread(int id)
{
	SafeWrite("Thread", id, "start");
	WaitForSingleObject(hTimer, INFINITE);
	SafeWrite("Thread", id, "end");
}

int main()
{
	hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	const int nThreads = 10;
	for (int i = 0; i < nThreads; ++i)
	{
		thread t(MyThread, i);
		t.detach();
	}

	LARGE_INTEGER dTime;
	dTime.QuadPart = -10000*1000;
	SetWaitableTimer(hTimer, &dTime, 1000, NULL, NULL, false);
	SafeWrite("Threads created");
	_getch();



	//	FILETIME uTime, fTime;
	//	SYSTEMTIME sTime = { 0 };
	//	SystemTimeToFileTime(&sTime, &fTime);
	//	LocalFileTimeToFileTime(&fTime, &uTime);
	//	SetWaitableTimer(hTimer, (LARGE_INTEGER*)&uTime, 1000, NULL, NULL, false);
	//	_getch();

	CancelWaitableTimer(hTimer);
	CloseHandle(hTimer);

	return 0;
}
