#pragma once

#include "ITransportDevyatkina.h"
#include "LocalTransportDevyatkina.h"

class BrokerTransportDevyatkina : public ITransportDevyatkina
{
public:
	BrokerTransportDevyatkina() = default;

	virtual void send(MessageDevyatkina& m) const override;
	virtual void receive(MessageDevyatkina& m) const override;
};

