#pragma once

#include "ITransportDevyatkina.h"
#include "SessionDevyatkina.h"
#include "TransportApiDevyatkina.h"
#include <map>
#include <memory>
#include <mutex>

class LAB4DEVYATKINA_TRANSPORTDLL_API LocalTransportDevyatkina : public ITransportDevyatkina
{
public:
	int id = -1;

	LocalTransportDevyatkina(int id = -1);

	static std::map<int, std::shared_ptr<SessionDevyatkina>> sessions;
	static std::mutex mx;

	static void setSession(int id, std::shared_ptr<SessionDevyatkina> session);
	static void eraseSession(int id);
	static std::shared_ptr<SessionDevyatkina> getSession(int id);

	virtual void send(MessageDevyatkina& m) const override;
	virtual void receive(MessageDevyatkina& m) const override;
};

