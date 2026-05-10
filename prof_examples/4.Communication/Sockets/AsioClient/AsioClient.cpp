#include "../AsioServer/asio.h"

int main()
{
	std::locale::global(std::locale("rus_rus.866"));
	wcin.imbue(std::locale());
	wcout.imbue(std::locale());

	try
	{
		boost::asio::io_context io;
		tcp::socket s(io);
		tcp::resolver r(io);
		boost::asio::connect(s, r.resolve("127.0.0.1", "12345"));

		while (true)
		{
			wstring str;
			wcin >> str;
			sendString(s, str);
			if (str == L"quit")
			{
				break;
			}
			wcout << receiveString(s) << endl;
		}
	}
	catch (std::exception& e)
	{
		std::wcerr << "Exception: " << e.what() << endl;
	}
}
