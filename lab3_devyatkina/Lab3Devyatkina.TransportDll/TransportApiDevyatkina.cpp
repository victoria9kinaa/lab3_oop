#include "TransportApiDevyatkina.h"
#include "SocketTransportDevyatkina.h"
#include "ProtocolDevyatkina.h"
#include "SysProgDevyatkina.h"

static SocketTransportDevyatkina g_ClientTransportDevyatkina;
static std::mutex g_ClientMxDevyatkina;

static int ParseIntDevyatkina(const std::wstring& s)
{
	try
	{
		return std::stoi(s);
	}
	catch (...)
	{
		return -1;
	}
}

static int ExchangeCountDevyatkina(MessageDevyatkina& req)
{
	if (!g_ClientTransportDevyatkina.isConnected())
		return -1;

	try
	{
		req.send(g_ClientTransportDevyatkina);
		MessageDevyatkina resp = MessageDevyatkina::receiveMessage(g_ClientTransportDevyatkina);
		if (resp.header.type != MT_INFO_DEVYATKINA)
			return -1;
		return ParseIntDevyatkina(resp.data);
	}
	catch (...)
	{
		return -1;
	}
}

int Devyatkina_Connect()
{
	lock_guard lg(g_ClientMxDevyatkina);
	if (g_ClientTransportDevyatkina.isConnected())
	{
		// Get thread count without nested mutex call
		MessageDevyatkina req(MT_INFO_DEVYATKINA, L"");
		return ExchangeCountDevyatkina(req);
	}

	if (!g_ClientTransportDevyatkina.connect())
		return -1;

	try
	{
		MessageDevyatkina resp = MessageDevyatkina::receiveMessage(g_ClientTransportDevyatkina);
		if (resp.header.type != MT_INFO_DEVYATKINA)
			return -1;
		return ParseIntDevyatkina(resp.data);
	}
	catch (...)
	{
		return -1;
	}
}

void Devyatkina_Disconnect()
{
	lock_guard lg(g_ClientMxDevyatkina);
	g_ClientTransportDevyatkina.close();
}

int Devyatkina_RequestThreadCount()
{
	lock_guard lg(g_ClientMxDevyatkina);
	MessageDevyatkina req(MT_INFO_DEVYATKINA, L"");
	return ExchangeCountDevyatkina(req);
}

int Devyatkina_StartThreads(int nThreads)
{
	lock_guard lg(g_ClientMxDevyatkina);
	MessageDevyatkina req(MT_START_DEVYATKINA, std::to_wstring(nThreads));
	return ExchangeCountDevyatkina(req);
}

int Devyatkina_StopThread()
{
	lock_guard lg(g_ClientMxDevyatkina);
	MessageDevyatkina req(MT_STOP_DEVYATKINA, L"");
	return ExchangeCountDevyatkina(req);
}

int Devyatkina_SendText(int to, const wchar_t* text)
{
	lock_guard lg(g_ClientMxDevyatkina);
	if (!g_ClientTransportDevyatkina.isConnected())
		return 0;
	if (!text)
		return 0;

	try
	{
		MessageDevyatkina req(MT_DATA_DEVYATKINA, text, to, 0);
		req.send(g_ClientTransportDevyatkina);
		MessageDevyatkina resp = MessageDevyatkina::receiveMessage(g_ClientTransportDevyatkina);
		return resp.header.type == MT_ACK_DEVYATKINA;
	}
	catch (...)
	{
		return 0;
	}
}

