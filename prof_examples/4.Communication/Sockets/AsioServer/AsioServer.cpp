#include "asio.h"

void launchClient()
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	wchar_t path[] = L"AsioClient.exe";
	CreateProcess(NULL, path, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

void processClient(tcp::socket s)
{
	try
	{
		while (true)
		{
			wstring str = receiveString(s);
			wcout << str << endl;
			if (str == L"quit")
			{
				break;
			}
			sendString(s, L"ok");
		}
	}
	catch (std::exception& e)
	{
		std::wcerr << "Client exception: " << e.what() << endl;
	}
}

int main()
{
	std::locale::global(std::locale("rus_rus.866"));
	wcin.imbue(std::locale());
	wcout.imbue(std::locale());
			
	try
	{
		int port = 12345;
		boost::asio::io_context io;
		tcp::acceptor a(io, tcp::endpoint(tcp::v4(), port));
		
		for (int i = 0; i < 3; ++i)
		{
			launchClient();
		}

		while (true)
		{
			std::thread(processClient, a.accept()).detach();
		}
	}
	catch (std::exception& e)
	{
		std::wcerr << "Exception: " << e.what() << endl;
	}
}
