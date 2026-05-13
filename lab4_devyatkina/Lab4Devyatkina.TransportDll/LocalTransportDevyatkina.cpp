#include "LocalTransportDevyatkina.h"

std::map<int, std::shared_ptr<SessionDevyatkina>> LocalTransportDevyatkina::sessions;
std::mutex LocalTransportDevyatkina::mx;

LocalTransportDevyatkina::LocalTransportDevyatkina(int id)
	:id(id)
{
}

void LocalTransportDevyatkina::setSession(int id, std::shared_ptr<SessionDevyatkina> session)
{
	std::lock_guard<std::mutex> lg(mx);
	sessions[id] = session;
}

void LocalTransportDevyatkina::eraseSession(int id)
{
	std::lock_guard<std::mutex> lg(mx);
	sessions.erase(id);
}

std::shared_ptr<SessionDevyatkina> LocalTransportDevyatkina::getSession(int id)
{
	std::lock_guard<std::mutex> lg(mx);
	auto it = sessions.find(id);
	if (it == sessions.end())
		return nullptr;
	return it->second;
}

void LocalTransportDevyatkina::send(MessageDevyatkina& m) const
{
	if (id < 0)
	{
		auto s = getSession(m.header.to);
		if (s)
			s->addMessage(m);
		return;
	}
	auto s = getSession(id);
	if (s)
		s->addMessage(m);
}

void LocalTransportDevyatkina::receive(MessageDevyatkina& m) const
{
	auto s = getSession(id);
	if (!s)
		return;
	s->getMessage(m);
}

