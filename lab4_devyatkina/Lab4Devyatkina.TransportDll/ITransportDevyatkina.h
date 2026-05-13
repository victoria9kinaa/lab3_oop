#pragma once

#include "TransportApiDevyatkina.h"

struct MessageDevyatkina;

struct LAB4DEVYATKINA_TRANSPORTDLL_API ITransportDevyatkina
{
	virtual ~ITransportDevyatkina() = default;
	virtual void send(MessageDevyatkina& m) const = 0;
	virtual void receive(MessageDevyatkina& m) const = 0;
};

