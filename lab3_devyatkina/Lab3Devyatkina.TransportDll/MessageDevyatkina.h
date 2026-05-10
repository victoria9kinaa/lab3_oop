#pragma once

#include <string>
#include "HeaderDevyatkina.h"
#include "TransportApiDevyatkina.h"

struct ITransportDevyatkina;

struct LAB3DEVYATKINA_TRANSPORTDLL_API MessageDevyatkina
{
	HeaderDevyatkina header = { 0 };
	std::wstring data;

	MessageDevyatkina() = default;
	MessageDevyatkina(int type, const std::wstring& data = L"", int to = 0, int from = 0);

	void send(const ITransportDevyatkina& transport);
	void receive(const ITransportDevyatkina& transport);

	static void sendMessage(const ITransportDevyatkina& transport, int type, const std::wstring& data = L"", int to = 0, int from = 0);
	static MessageDevyatkina receiveMessage(const ITransportDevyatkina& transport);
};

