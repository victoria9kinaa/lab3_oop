#pragma once

#include "Message.h"

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
