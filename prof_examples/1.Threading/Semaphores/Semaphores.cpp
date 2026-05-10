#include "../Common/SysProg.API.h"

HANDLE hSemaphore;

void MyThread(int id)
{
	SafeWrite("Thread", id, "start");

	WaitForSingleObject(hSemaphore, INFINITE);
	SafeWrite("Thread", id, "semaphore capture");
	Sleep(3000);
	ReleaseSemaphore(hSemaphore, 1, NULL);
	SafeWrite("Thread", id, "semaphore release");

	SafeWrite("Thread", id, "end");
}

int main()
{
	hSemaphore = CreateSemaphore(NULL, 1, 3, NULL);
	const int nThreads = 10;
	for (int i = 0; i < nThreads; ++i)
	{
		thread t(MyThread, i);
		t.detach();
	}
	SafeWrite("Threads created");
	int n;
	cin >> n;
	ReleaseSemaphore(hSemaphore, n, NULL);
	_getch();

	CloseHandle(hSemaphore);

	return 0;
}
