#include "../Common/SysProg.h"

struct Message;
	
class Sender
{
public:
	virtual void send(Message&) const = 0;
	//	virtual void operator()(Message&) const = 0;
};

class Receiver
{
public:
	virtual void receive(Message&) const = 0;
	//	virtual void operator()(Message&) const = 0;
};

enum MessageTypes
{
	MT_CLOSE,
	MT_DATA,
};

struct MessageHeader
{
	int messageType;
	int size;
	int to;
	int from;
};

struct Message
{
	MessageHeader header = { 0 };
	wstring data;
	Message() = default;
	Message(MessageTypes messageType, const wstring& data = L"")
		:data(data)
	{
		header = {messageType,  int(data.length() * sizeof(wchar_t)) };
	}

	Message(int to, MessageTypes messageType, const wstring& data = L"")
		:data(data)
	{
		header = { messageType,  int(data.length() * sizeof(wchar_t)), to };
	}

	void send(const Sender& sender)
	{
		sender.send(*this);
		// sender(*this);
	}

	void receive(const Receiver& receiver)
	{
		receiver.receive(*this);
		// receiver(*this);
	}

	static void sendMessage(const Sender& sender, MessageTypes messageType, const wstring& data = L"")
	{
		Message m(messageType, data);
		m.send(sender);
	}

	static void sendMessage(const Sender& sender, int to, MessageTypes messageType, const wstring& data = L"")
	{
		Message m(to, messageType, data);
		m.send(sender);
	}

	static Message receiveMessage(const Receiver& receiver)
	{
		Message m;
		m.receive(receiver);
		return m;
	}
};

class Session
{
	queue<Message> messages;
	mutex mx;
	condition_variable ev;

public:
	int sessionID;

	Session(int sessionID)
		:sessionID(sessionID)
	{
	}

	void addMessage(Message& m)
	{
		{
			lock_guard lg(mx);
			messages.push(m);
		}
		ev.notify_one();
	}

	void getMessage(Message& m)
	{
		unique_lock sl(mx);
		ev.wait(sl, [&] {return !messages.empty();});
		m = messages.front();
		messages.pop();
	}
};

class SRLocal: public Sender, public Receiver
{
public:
	int id;

	SRLocal(int id = -1)
		:id(id)
	{ }

	static inline vector<jthread> threads;
	static inline map<int, Session*> sessions;
	static inline mutex mx;

	virtual void send(Message& m) const
	{
		if(id < 0)
			sessions[m.header.to]->addMessage(m);
		else
			sessions[id]->addMessage(m);
	}
	virtual void receive(Message& m) const
	{
		sessions[id]->getMessage(m);
	}

	static void worker(int sessionID)
	{
		auto session = new Session(sessionID);
		mx.lock();
		sessions[sessionID] = session;
		mx.unlock();
		SafeWrite("session", session->sessionID, "created");
		while (true)
		{
			Message m = Message::receiveMessage(SRLocal(sessionID));
			switch (m.header.messageType)
			{
				case MT_CLOSE:
				{
					lock_guard lg(mx);
					sessions.erase(sessionID);
					delete session;
					SafeWrite("session", sessionID, "closed");
					return;
				}
				case MT_DATA:
				{
					SafeWrite("session", sessionID, "data", m.data);
					Sleep(500 * sessionID);
					break;
				}
			}
		}
	}

	static void addThread(int sessionID)
	{
		threads.emplace_back(worker, sessionID);
	}

	static void waitThreads()
	{
		threads.clear();
	}
};

int main()
{
	std::locale::global(std::locale("rus_rus.866"));
	wcin.imbue(std::locale());
	wcout.imbue(std::locale());

	SafeWrite("enter n");
	int n;
	cin >> n;
	for (int i = 0; i < n; ++i)
	{
		SRLocal::addThread(i);
	}

	SafeWrite("creation done");
	_getch();

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			Message::sendMessage(SRLocal(i), MT_DATA, to_wstring(i));
		}
	}

	SafeWrite("sending done");
	_getch();

	for (int i = 0; i < n; ++i)
	{
		Message::sendMessage(SRLocal(), i, MT_CLOSE);
	}

	SafeWrite("closing done");

	SRLocal::waitThreads();

	SafeWrite("done");
	_getch();

	return 0;
}
