#include "../Common/SysProg.API.h"

enum MessageTypes
{
	MT_CLOSE,
	MT_DATA,
};

struct MessageHeader
{
	int messageType;
	int size;
};

struct Message
{
	MessageHeader header = { 0 };
	wstring data;
	Message() = default;
	Message(MessageTypes messageType, const wstring& data = L"")
		:data(data)
	{
		header = { messageType,  int(data.length() * sizeof(wchar_t)) };
	}
};

class Session
{
	queue<Message> messages;
	CRITICAL_SECTION cs;
	HANDLE hEvent;
public:
	int sessionID;

	Session(int sessionID)
		:sessionID(sessionID)
	{
		InitializeCriticalSection(&cs);
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	~Session()
	{
		DeleteCriticalSection(&cs);
		CloseHandle(hEvent);
	}

	void addMessage(Message& m)
	{
		EnterCriticalSection(&cs);
		messages.push(m);
		SetEvent(hEvent);
		LeaveCriticalSection(&cs);
	}

	bool getMessage(Message& m)
	{
		bool res = false;
		WaitForSingleObject(hEvent, INFINITE);
		EnterCriticalSection(&cs);
		if (!messages.empty())
		{
			res = true;
			m = messages.front();
			messages.pop();
		}
		if (messages.empty())
		{
			ResetEvent(hEvent);
		}
		LeaveCriticalSection(&cs);
		return res;
	}

	void addMessage(MessageTypes messageType, const wstring& data = L"")
	{
		Message m(messageType, data);
		addMessage(m);
	}
};

DWORD WINAPI MyThread(LPVOID lpParameter)
{
	auto session = static_cast<Session*>(lpParameter);
	SafeWrite("session", session->sessionID, "created");
	while (true)
	{
		Message m;
		if (session->getMessage(m))
		{
			switch (m.header.messageType)
			{
				case MT_CLOSE:
				{
					SafeWrite("session", session->sessionID, "closed");
					delete session;
					return 0;
				}
				case MT_DATA:
				{
					SafeWrite("session", session->sessionID, "data", m.data);
					Sleep(500 * session->sessionID);
					break;
				}
			}
		}
	}
	return 0;
}

int main()
{
	std::locale::global(std::locale("rus_rus.866"));
	wcin.imbue(std::locale());
	wcout.imbue(std::locale());

	SafeWrite("enter n");
	int n;
	cin >> n;
	vector<Session*> sessions;
	vector<HANDLE> threads;
	for (int i = 0; i < n; ++i)
	{
		sessions.push_back(new Session(i));
		threads.push_back(CreateThread(NULL, 0, MyThread, (LPVOID)sessions.back(), 0, NULL));
	}

	SafeWrite("creation done");
	_getch();

	for (auto s : sessions)
	{
		for (int i = 0; i < 10; ++i)
		{
			s->addMessage(MT_DATA, to_wstring(i));
		}
	}

	SafeWrite("sending done");
	_getch();

	for (auto s : sessions)
	{
		s->addMessage(MT_CLOSE);
	}

	SafeWrite("closing done");

	WaitForMultipleObjects((DWORD)threads.size(), threads.data(), TRUE, INFINITE);

	SafeWrite("done");
	_getch();

	return 0;
}
