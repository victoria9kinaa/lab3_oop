#include "TransportApiDevyatkina.h"
#include "SocketTransportDevyatkina.h"
#include "ProtocolDevyatkina.h"

#include <combaseapi.h>
#include <cwchar>
#include <mutex>

static SocketTransportDevyatkina g_clientTransportDevyatkina;
static std::mutex g_clientStateMxDevyatkina;
static std::mutex g_sendMxDevyatkina;
static std::mutex g_receiveMxDevyatkina;

int Devyatkina_Connect()
{
	std::lock_guard<std::mutex> lg(g_clientStateMxDevyatkina);
	if (g_clientTransportDevyatkina.isConnected())
		return 1;

	if (!g_clientTransportDevyatkina.connect())
		return 0;

	try
	{
		MessageDevyatkina req(MT_CONNECT_DEVYATKINA, L"", -2, 0);
		req.send(g_clientTransportDevyatkina);
		return 1;
	}
	catch (...)
	{
		g_clientTransportDevyatkina.close();
		return 0;
	}
}

void Devyatkina_Disconnect()
{
	std::lock_guard<std::mutex> lg(g_clientStateMxDevyatkina);
	if (!g_clientTransportDevyatkina.isConnected())
		return;

	try
	{
		MessageDevyatkina req(MT_DISCONNECT_DEVYATKINA, L"", -2, 0);
		req.send(g_clientTransportDevyatkina);
	}
	catch (...)
	{
	}

	g_clientTransportDevyatkina.close();
}

int Devyatkina_SendMessage(MsgStructDevyatkina msg)
{
	std::lock_guard<std::mutex> lg(g_sendMxDevyatkina);
	if (!g_clientTransportDevyatkina.isConnected())
		return 0;

	try
	{
		std::wstring data = msg.data ? msg.data : L"";
		MessageDevyatkina req(static_cast<int>(msg.msgType), data, msg.to, 0);
		req.send(g_clientTransportDevyatkina);
		return 1;
	}
	catch (...)
	{
		g_clientTransportDevyatkina.close();
		return 0;
	}
}

BOOL Devyatkina_ReceiveMessage(MsgStructDevyatkina* msg)
{
	if (!msg)
		return FALSE;

	std::lock_guard<std::mutex> lg(g_receiveMxDevyatkina);
	if (!g_clientTransportDevyatkina.isConnected())
		return FALSE;

	try
	{
		MessageDevyatkina m = MessageDevyatkina::receiveMessage(g_clientTransportDevyatkina);
		msg->to = m.header.to;
		msg->msgType = static_cast<MessageTypesDevyatkina>(m.header.type);

		if (m.header.type == MT_DISCONNECT_DEVYATKINA)
		{
			g_clientTransportDevyatkina.close();
			return FALSE;
		}

		size_t sizeInBytes = (m.data.length() + 1) * sizeof(wchar_t);
		wchar_t* buffer = static_cast<wchar_t*>(CoTaskMemAlloc(sizeInBytes));
		if (!buffer)
			return FALSE;

		wcscpy_s(buffer, m.data.length() + 1, m.data.c_str());
		msg->data = buffer;
		return TRUE;
	}
	catch (...)
	{
		g_clientTransportDevyatkina.close();
		return FALSE;
	}
}

