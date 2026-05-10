#include <iostream>
#include <windows.h>
using namespace std;

int main(int argc, char** argv)
{
	wcin.imbue(std::locale("rus_rus.866"));
	wcout.imbue(std::locale("rus_rus.866"));

	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	const int MAXLEN = 1024;

	bool fromSharp = argc > 1;
	while (true)
	{
		DWORD dwRead;
		char buff[MAXLEN+1];

		if (!ReadFile(hIn, buff, MAXLEN, &dwRead, nullptr) || !dwRead)
			break;

		if (fromSharp)
		{
			wchar_t* wbuff = (wchar_t*)buff;
			wbuff[min(MAXLEN, dwRead/2)] = 0;
			wcout << L"c# child:" << dwRead << " " << wbuff << endl;
		}
		else
		{
			buff[min(MAXLEN, dwRead)] = 0;
			cout << "console child: " << dwRead << " " << buff << endl;
		}
	} 
}
