#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCKAPI_

#include "SysProg.h"
#include <map>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>

#include "../Lab4Devyatkina.TransportDll/LocalTransportDevyatkina.h"
#include "../Lab4Devyatkina.TransportDll/SocketTransportDevyatkina.h"
#include "../Lab4Devyatkina.TransportDll/SessionDevyatkina.h"
#include "../Lab4Devyatkina.TransportDll/ProtocolDevyatkina.h"

class ServerDevyatkina
{
    // ИСПРАВЛЕНО: recursive_mutex вместо mutex — устраняет дедлок
    // broadcastSessions() -> sendSessionsToClient() -> buildSessionsList()
    // все берут один мьютекс внутри друг друга
    static std::recursive_mutex m_mx;

    static std::map<int, std::shared_ptr<SessionDevyatkina>> m_sessions;
    static std::map<int, std::shared_ptr<boost::asio::ip::tcp::socket>> m_clients;
    static std::map<int, std::thread> m_workers;
    static std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
    static int m_nextClientId;
    static std::chrono::milliseconds m_timeout;

    static void worker(int clientId);
    static void clientHandler(int clientId, std::shared_ptr<boost::asio::ip::tcp::socket> socket);
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
