#include "../AsioCommon/Message.2.h"

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

	bool getMessage(Message& m)
	{
		lock_guard<mutex> lg(mx);
		if (messages.empty())
		{
			return false;
		}
		else
		{
			m = messages.front();
			messages.pop();
			return true;
		}
	}
};


class SRBroker: public Sender, public Receiver
{
public:
	SRBroker() = default;

	static inline int maxID = MR_USER;
	static inline map<int, shared_ptr<Session>> sessions;
	static inline mutex mx;

	virtual void send(Message& m) const
	{
		lock_guard lg(mx);
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
		}
	}

	virtual void receive(Message& m) const
	{
		lock_guard lg(mx);
		auto iSession = sessions.find(m.header.from);
		if (iSession == sessions.end() || ! iSession->second->getMessage(m))
			m = { m.header.from, MR_BROKER, MT_NODATA };
	}

	static void worker(tcp::socket s)
	{
		while (true)
		{
			try
			{
				Message m = Message::receiveMessage(SRSocket(s));
				safeWrite(m.header.to, m.header.from, m.header.type);
				switch (m.header.type)
				{
					case MT_INIT:
					{
						lock_guard lg(mx);
						auto session = make_shared<Session>(++maxID, m.data);
						sessions[session->sessionID] = session;
						Message(session->sessionID, MR_BROKER, MT_INIT).send(SRSocket(s));
						break;
					}
					case MT_EXIT:
					{
						lock_guard lg(mx);
						sessions.erase(m.header.from);
						Message(m.header.from, MR_BROKER, MT_CONFIRM).send(SRSocket(s));
						break;
					}
					case MT_GETDATA:
					{
						m.receive(SRBroker()).send(SRSocket(s));
						break;
					}
					default:
					{
						m.send(SRBroker());
						Message(m.header.from, MR_BROKER, MT_CONFIRM).send(SRSocket(s));
						break;
					}
				}
			}
			catch (std::exception&)
			{
//				std::wcerr << "Exception: " << e.what() << endl;
				break;
			}
		}
	}
};


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
			std::thread(SRBroker::worker, a.accept()).detach();
		}
	}
	catch (std::exception& e)
	{
		std::wcerr << "Exception: " << e.what() << endl;
	}

	return 0;
}
