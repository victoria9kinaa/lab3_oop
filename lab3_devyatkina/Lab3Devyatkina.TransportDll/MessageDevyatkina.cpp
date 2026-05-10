#include "MessageDevyatkina.h"
#include "ITransportDevyatkina.h"

MessageDevyatkina::MessageDevyatkina(int type, const std::wstring& data, int to, int from)
	:data(data)
{
	header.type = type;
	header.size = int(data.length() * sizeof(wchar_t));
	header.to = to;
	header.from = from;
}

void MessageDevyatkina::send(const ITransportDevyatkina& transport)
{
	transport.send(*this);
}

void MessageDevyatkina::receive(const ITransportDevyatkina& transport)
{
	transport.receive(*this);
}

void MessageDevyatkina::sendMessage(const ITransportDevyatkina& transport, int type, const std::wstring& data, int to, int from)
{
	MessageDevyatkina m(type, data, to, from);
	m.send(transport);
}

MessageDevyatkina MessageDevyatkina::receiveMessage(const ITransportDevyatkina& transport)
{
	MessageDevyatkina m;
	m.receive(transport);
	return m;
}

