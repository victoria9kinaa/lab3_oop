#pragma once

// Define Windows macros to prevent conflicts
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCKAPI_

#include "SysProg.h"
#include <memory>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <iostream>
#include <fstream>

#include "../Lab3Devyatkina.TransportDll/MMFTransportDevyatkina.h"
#include "../Lab3Devyatkina.TransportDll/LocalTransportDevyatkina.h"
#include "../Lab3Devyatkina.TransportDll/SocketTransportDevyatkina.h"
#include "../Lab3Devyatkina.TransportDll/SessionDevyatkina.h"
#include "../Lab3Devyatkina.TransportDll/ProtocolDevyatkina.h"

class ServerDevyatkina
{
	static std::vector<jthread> m_threads;
	static std::map<int, SessionDevyatkina*> m_sessions;
	static std::mutex m_mx;
	static std::shared_ptr<tcp::acceptor> m_acceptor;
	static int m_nextThreadId;

	static void worker(int threadId);
	static void handleClientMessage(const MessageDevyatkina& msg, SocketTransportDevyatkina& clientTransport);

public:
	ServerDevyatkina();
	void run();
};

