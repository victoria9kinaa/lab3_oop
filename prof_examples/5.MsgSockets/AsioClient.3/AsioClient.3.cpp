#include "../AsioCommon/Message.2.h"

void processMessages(tcp::socket& s)
{
	try
	{
		Message(MR_BROKER, MT_INIT).send(SRSocket(s, true));
		while (true)
		{
			Message m = Message::sendMessage(SRSocket(s, true), MR_BROKER, MT_GETDATA);
			switch (m.header.type)
			{
				case MT_DATA:
					safeWrite(m.data);
				default:
					Sleep(100);
					break;
			}
		}
	}
	catch (std::exception& e)
	{
		std::wcerr << "Exception: " << e.what() << endl;
	}
}

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

		thread(processMessages, std::ref(s)).detach();

		while (true)
		{
			wstring str;
			wcin >> str;
			Message(MR_ALL, MT_DATA, str).send(SRSocket(s, true));
		}

	}
	catch (std::exception& e)
	{
		std::wcerr << "Exception: " << e.what() << endl;
	}

	return 0;
}
