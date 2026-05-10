// MailSlotClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

int main()
{
	std::locale::global(std::locale("rus_rus.866"));
	wcin.imbue(std::locale());
	wcout.imbue(std::locale());

	//
	// \\.\mailslot\<slotname>
	// \\<systemname>\mailslot\<slotname>
	// \\*\mailslot\<slotname>
	// \\<domainname>\mailslot\<slotname>
	//

	HANDLE hMail = CreateFile(L"\\\\.\\mailslot\\myslot", 
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	wstring s;
	while (1)
	{
		wcin >> s;
		if (s == L"bye")
			break;
		DWORD dwWrite;
		WriteFile(hMail, s.c_str(), DWORD((s.length() + 1) * sizeof(wchar_t)), &dwWrite, NULL);
	}
	CloseHandle(hMail);
}

