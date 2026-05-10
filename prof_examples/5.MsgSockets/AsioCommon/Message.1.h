#pragma once

#include "asio.h"
#include "enums.h"

class Message
{
public:
	MessageHeader header = {0};
	wstring data;
	static inline int clientID = 0;

	Message() {}
	Message(int to, int from, int type = MT_DATA, const wstring& data = L"")
		:data(data)
	{
		header = {type, int(data.length() * sizeof(wchar_t)), to, from};
	}
	
	void send(tcp::socket& s)
	{
		sendData(s, &header);
		if (header.size)
		{
			sendData(s, data.data(), header.size);
		}
	}

	int receive(tcp::socket& s)
	{
		receiveData(s, &header);
		if (header.size)
		{
			data.resize(header.size / sizeof(wchar_t));
			receiveData(s, data.data(), header.size);
		}
		return header.type;
	}

	static void send(tcp::socket& s, int to, int from, int type = MT_DATA, const wstring& data = L"")
	{
		Message m(to, from, type, data);
		m.send(s);
	}
	
	static Message send(int to, int type = MT_DATA, const wstring& data = L"")
	{
		boost::asio::io_context io;
		tcp::socket s(io);
		tcp::resolver r(io);
		boost::asio::connect(s, r.resolve("127.0.0.1", "12345"));

		Message m(to, clientID, type, data);
		m.send(s);
		if (m.receive(s) == MT_INIT)
		{
			clientID = m.header.to;
		}
		return m;
	}
};

