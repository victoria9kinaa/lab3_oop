#include <windows.h>
#include <conio.h>
#include <iostream>
using namespace std;

int main()
{
	SECURITY_ATTRIBUTES sa = { 0 };
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	sa.bInheritHandle = TRUE;

	HANDLE hMutex1 = CreateMutex(&sa, TRUE, L"MyMutex");
	HANDLE hMutex2 = CreateMutex(&sa, TRUE, L"MyMutex");
	HANDLE hMutex3 = OpenMutex(0, FALSE, L"MyMutex");

	//wchar_t path[] = (LPWSTR)L"notepad";
	wchar_t path[] = L"Events.Sharp.exe";

	if (CreateProcess(NULL, path, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		DWORD code;
		GetExitCodeProcess(pi.hProcess, &code);
		std::cout << "exit code non closed " << code << std::endl;

		_getch();
		TerminateProcess(pi.hProcess, 12345);

		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &code);
		std::cout << "closed " << code << std::endl;

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	_getch();

	return 0;
}
