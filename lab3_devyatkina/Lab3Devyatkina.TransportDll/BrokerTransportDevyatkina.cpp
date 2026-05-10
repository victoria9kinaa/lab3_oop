#include "BrokerTransportDevyatkina.h"

void BrokerTransportDevyatkina::send(MessageDevyatkina& m) const
{
	LocalTransportDevyatkina().send(m);
}

void BrokerTransportDevyatkina::receive(MessageDevyatkina& m) const
{
	LocalTransportDevyatkina(m.header.to).receive(m);
}

