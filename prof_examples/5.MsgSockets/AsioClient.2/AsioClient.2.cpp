#include "../AsioCommon/Message.2.h"

void processMessages()
{
	try
	{
		Message(MR_BROKER, MT_INIT).send(SRSocket(true));
		while (true)
		{
			Message m = Message::sendMessage(SRSocket(true), MR_BROKER, MT_GETDATA);
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
		thread (processMessages).detach();

		while (true)
		{
			wstring str;
			wcin >> str;
			Message(MR_ALL, MT_DATA, str).send(SRSocket(true));
		}

	}
	catch (std::exception& e)
	{
		std::wcerr << "Exception: " << e.what() << endl;
	}

	return 0;
}
