#include "SessionDevyatkina.h"

SessionDevyatkina::SessionDevyatkina(int sessionID)
	:sessionID(sessionID)
{
}

void SessionDevyatkina::addMessage(MessageDevyatkina& m)
{
	{
		std::lock_guard<std::mutex> lg(m_mx);
		m_messages.push(m);
	}
	m_ev.notify_one();
}

void SessionDevyatkina::addMessage(int type, const std::wstring& data, int to, int from)
{
	MessageDevyatkina m(type, data, to, from);
	addMessage(m);
}

bool SessionDevyatkina::getMessage(MessageDevyatkina& m)
{
	std::unique_lock<std::mutex> ul(m_mx);
	m_ev.wait(ul, [&] {return !m_messages.empty(); });

	m = m_messages.front();
	m_messages.pop();
	return true;
}

