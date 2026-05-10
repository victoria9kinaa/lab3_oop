#include "../AsioCommon/Message.1.h"

class Session
{
	queue<Message> messages;
	mutex mx;

public:
	int sessionID;
	wstring name;

	Session(int sessionID, wstring name)
		:sessionID(sessionID), name(name)
	{
	}

	void addMessage(Message& m)
	{
		lock_guard<mutex> lg(mx);
		messages.push(m);
	}

	void sendMessage(tcp::socket& s)
	{
		lock_guard<mutex> lg(mx);
		if (messages.empty())
		{
			Message::send(s, sessionID, MR_BROKER, MT_NODATA);
		}
		else
		{
			messages.front().send(s);
			messages.pop();
		}
	}
};


int maxID = MR_USER;
map<int, shared_ptr<Session>> sessions;

void processClient(tcp::socket s)
{
	while (true)
	{
		try
		{
			Message m;
			int code = m.receive(s);
			safeWrite(m.header.to, m.header.from, m.header.type, code);
			switch (code)
			{
				case MT_INIT:
				{
					auto session = make_shared<Session>(++maxID, m.data);
					sessions[session->sessionID] = session;
					Message::send(s, session->sessionID, MR_BROKER, MT_INIT);
					break;
				}
				case MT_EXIT:
				{
					sessions.erase(m.header.from);
					Message::send(s, m.header.from, MR_BROKER, MT_CONFIRM);
					break;
				}
				case MT_GETDATA:
				{
					auto iSession = sessions.find(m.header.from);
					if (iSession != sessions.end())
					{
						iSession->second->sendMessage(s);
					}
					break;
				}
				default:
				{
					auto iSessionFrom = sessions.find(m.header.from);
					if (iSessionFrom != sessions.end())
					{
						auto iSessionTo = sessions.find(m.header.to);
						if (iSessionTo != sessions.end())
						{
							iSessionTo->second->addMessage(m);
						}
						else if (m.header.to == MR_ALL)
						{
							for (auto& [id, session] : sessions)
							{
								if (id != m.header.from)
									session->addMessage(m);
							}
						}
						Message::send(s, m.header.from, MR_BROKER, MT_CONFIRM);
					}
					break;
				}
			}
		}
		catch (std::exception&)
		{
//			std::wcerr << "Exception: " << e.what() << endl;
			break;
		}
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

		launchClient(L"AsioClient.1.exe");
		launchClient(L"AsioClient.2.exe");
		launchClient(L"AsioClient.3.exe");
		launchClient(L"SharpClient.exe");

		while (true)
		{
			std::thread(processClient, a.accept()).detach();
		}
	}
	catch (std::exception& e)
	{
		std::wcerr << "Exception: " << e.what() << endl;
	}

	return 0;
}
