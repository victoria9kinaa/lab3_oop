#pragma once

// Define Windows macros to prevent conflicts
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCKAPI_

#include "SysProg.h"
#include <chrono>
#include <memory>
#include <thread>
#include <map>
#include <mutex>

#include "../Lab4Devyatkina.TransportDll/LocalTransportDevyatkina.h"
#include "../Lab4Devyatkina.TransportDll/SocketTransportDevyatkina.h"
#include "../Lab4Devyatkina.TransportDll/SessionDevyatkina.h"
#include "../Lab4Devyatkina.TransportDll/ProtocolDevyatkina.h"

class ServerDevyatkina
{
	static std::mutex m_mx;
	static std::map<int, std::shared_ptr<SessionDevyatkina>> m_sessions;
	static std::map<int, std::shared_ptr<tcp::socket>> m_clients;
	static std::map<int, jthread> m_workers;
	static std::shared_ptr<tcp::acceptor> m_acceptor;
	static int m_nextClientId;
	static std::chrono::milliseconds m_timeout;

	static void worker(int clientId);
	static void clientHandler(int clientId, std::shared_ptr<tcp::socket> socket);
	static void handleClientMessage(MessageDevyatkina& msg, int clientId);
	static void sendSessionsToClient(int clientId);
	static void broadcastSessions();
	static std::wstring buildSessionsList();
	static void removeClient(int clientId, bool notifyClient);
	static void monitorTimeouts();

public:
	ServerDevyatkina();
	void run();
};

