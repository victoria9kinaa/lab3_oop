#include "../Common/SysProg.API.h"

__declspec (thread) int gID = 0;

void f1(int id)
{
	__declspec (thread) static int sID = 0;
	sID = id;
	for (int i = 0; i < 2; ++i)
	{
		SafeWrite("Thread", id, "sID", sID, "gID", gID);
		Sleep(1000);
	}
}

void MyThread1(int id)
{
	SafeWrite("Thread", id, "start");
	gID = id;
	f1(id);
	SafeWrite("Thread", id, "end");
}

DWORD tlsIndex;

void f2()
{
	for (int i = 0; i < 2; ++i)
	{
		SafeWrite("tlsIndex", tlsIndex, "id from TLS", *((int*)TlsGetValue(tlsIndex)));
		Sleep(1000);
	}
}

void MyThread2(int id)
{
	SafeWrite("Thread", id, "start");

	if (!TlsGetValue(tlsIndex))
		TlsSetValue(tlsIndex, new int);

	*((int*)TlsGetValue(tlsIndex)) = id;
	f2();
	delete (int*)TlsGetValue(tlsIndex);

	SafeWrite("Thread", id, "end");
}

int main()
{
	tlsIndex = TlsAlloc();
	const int nThreads = 10;
	for (int i = 0; i < nThreads; ++i)
	{
		thread t(MyThread2, i);
		t.detach();
	}
	SafeWrite("Threads created");
	_getch();
	TlsFree(tlsIndex);

	return 0;
}
