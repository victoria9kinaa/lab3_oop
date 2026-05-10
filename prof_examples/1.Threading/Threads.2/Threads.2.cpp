#include "../Common/SysProg.API.h"

class MyClass
{
public:
	MyClass()
	{
		SafeWrite("MyClass::MyClass()");
	}
	~MyClass()
	{
		SafeWrite("MyClass::~MyClass()");
	}
};

void f(int id)
{
	MyClass o;
//	ExitThread(id*100);
	_endthreadex(id * 100);
}

DWORD WINAPI MyThreadAPI(LPVOID lpParam)
{
	int id = (int)lpParam;
	for (int i = 0; i < 10; ++i)
	{
		SafeWrite("API Thread ", id);
		Sleep(100);
	}
	f(id);
	SafeWrite("After f()");
	return id*10;
}

unsigned MyThreadC(void* lpParam)
{
	int id = (int)lpParam;
	for (int i = 0; i < 10; ++i)
	{
		SafeWrite("C Thread ", id);
		Sleep(100);
	}
	f(id);
	SafeWrite("After f()");
	return id*100;
}

void MyThreadStd(int id)
{
	for (int i = 0; i < 10; ++i)
	{
		SafeWrite("Std Thread ", id);
		Sleep(100);
	}
	f(id);
	SafeWrite("After f()");
}

int main()
{
	const int nThreads = 10;
	HANDLE hThreads[nThreads*3];
	for (int i = 0; i < nThreads; ++i)
	{
		hThreads[i] = CreateThread(NULL, 0, MyThreadAPI, (LPVOID)i, 0, NULL);
		hThreads[nThreads +i] = (HANDLE)_beginthreadex(NULL, 0, MyThreadC, (LPVOID)i, 0, NULL);
		thread t(MyThreadStd, i);
		hThreads[nThreads*2+i] = t.native_handle();
		t.detach();
//		t.join();
	}
	SafeWrite("Threads created");
	WaitForMultipleObjects(nThreads, hThreads, TRUE, INFINITE);
	SafeWrite("Threads done");

	for (int i = 0; i < nThreads*3; ++i)
	{
		DWORD dwCode;
		GetExitCodeThread(hThreads[i], &dwCode);
		SafeWrite(i, "exit code", dwCode);
		CloseHandle(hThreads[i]);
	}

	return 0;
}

