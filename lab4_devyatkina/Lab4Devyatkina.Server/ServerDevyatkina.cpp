#include "ServerDevyatkina.h"

std::recursive_mutex ServerDevyatkina::m_mx;
std::map<int, std::shared_ptr<SessionDevyatkina>> ServerDevyatkina::m_sessions;
std::map<int, std::shared_ptr<tcp::socket>> ServerDevyatkina::m_clients;
std::map<int, std::jthread> ServerDevyatkina::m_workers;
std::shared_ptr<tcp::acceptor> ServerDevyatkina::m_acceptor;
int ServerDevyatkina::m_nextClientId = 1;

ServerDevyatkina::ServerDevyatkina()
{
    try
    {
        m_acceptor = SocketTransportDevyatkina::listen(SocketTransportDevyatkina::SERVER_PORT);
        if (!m_acceptor)
        {
            SafeWrite("server", "failed to create acceptor");
            return;
        }
        SafeWrite("server", "acceptor created successfully");
    }
    catch (const std::exception& e) { SafeWrite("server", "acceptor creation failed:", e.what()); }
    catch (...) { SafeWrite("server", "acceptor creation failed: unknown error"); }
}

void ServerDevyatkina::run()
{
    SafeWrite("server", "start");

    while (true)
    {
        try
        {
            auto socket = SocketTransportDevyatkina::accept(m_acceptor);
            if (!socket) continue;

            int clientId = 0;
            {
                std::lock_guard<std::recursive_mutex> lg(m_mx);
                clientId = m_nextClientId++;
                auto session = std::make_shared<SessionDevyatkina>(clientId);
                m_sessions[clientId] = session;
                m_clients[clientId] = socket;
                LocalTransportDevyatkina::setSession(clientId, session);
                m_workers.try_emplace(clientId, &ServerDevyatkina::worker, clientId);
            }

            SafeWrite("session", clientId, "connected");
            std::thread(&ServerDevyatkina::clientHandler, clientId, socket).detach();
            broadcastSessions();
        }
        catch (const std::exception& e) { SafeWrite("server", "accept error:", e.what()); break; }
        catch (...) { SafeWrite("server", "accept error: unknown"); break; }
    }
}

void ServerDevyatkina::clientHandler(int clientId, std::shared_ptr<tcp::socket> socket)
{
    try
    {
        SocketTransportDevyatkina clientTransport(socket);
        while (clientTransport.isConnected())
        {
            try
            {
                MessageDevyatkina msg = MessageDevyatkina::receiveMessage(clientTransport);
                handleClientMessage(msg, clientId);
            }
            catch (...) { break; }
        }
    }
    catch (...) {}

    removeClient(clientId, true);
}

void ServerDevyatkina::handleClientMessage(MessageDevyatkina& msg, int clientId)
{
    switch (msg.header.type)
    {
    case MT_CONNECT_DEVYATKINA:
    {
        auto session = LocalTransportDevyatkina::getSession(clientId);
        if (session)
        {
            MessageDevyatkina confirm(MT_CONFIRM_DEVYATKINA, std::to_wstring(clientId), clientId, -2);
            session->addMessage(confirm);   // ← через очередь сессии
        }
        broadcastSessions();
        break;
    }

    case MT_DISCONNECT_DEVYATKINA:
        removeClient(clientId, false);
        break;

    case MT_INFO_DEVYATKINA:
        sendSessionsToClient(clientId);
        break;

    case MT_DATA_DEVYATKINA:
    {
        if (msg.header.to == -1)
        {
            // Отправить всем кроме отправителя
            std::vector<int> ids;
            {
                std::lock_guard<std::recursive_mutex> lg(m_mx);
                for (const auto& [id, _] : m_sessions)
                    if (id != clientId) ids.push_back(id);
            }
            for (int id : ids)
            {
                MessageDevyatkina out = msg;
                out.header.from = clientId;
                out.header.to = id;
                LocalTransportDevyatkina(id).send(out);
            }
        }
        else
        {
            // Отправить конкретному
            bool exists = false;
            {
                std::lock_guard<std::recursive_mutex> lg(m_mx);
                exists = m_sessions.count(msg.header.to) > 0;
            }
            if (exists)
            {
                MessageDevyatkina out = msg;
                out.header.from = clientId;
                LocalTransportDevyatkina(out.header.to).send(out);
            }
        }
        sendSessionsToClient(clientId);
        break;
    }
    }
}

void ServerDevyatkina::worker(int clientId)
{
    while (true)
    {
        MessageDevyatkina msg;
        LocalTransportDevyatkina(clientId).receive(msg);

        if (msg.header.type == MT_DISCONNECT_DEVYATKINA)
            break;

        std::shared_ptr<tcp::socket> socket;
        {
            std::lock_guard<std::recursive_mutex> lg(m_mx);
            auto it = m_clients.find(clientId);
            if (it != m_clients.end()) socket = it->second;
        }

        if (!socket) break;

        try
        {
            SocketTransportDevyatkina transport(socket);
            msg.send(transport);
        }
        catch (...) { break; }
    }

    
    SafeWrite("session", clientId, "worker finished");
}

void ServerDevyatkina::sendSessionsToClient(int clientId)
{
    auto session = LocalTransportDevyatkina::getSession(clientId);
    if (!session) return;

    MessageDevyatkina info(MT_INFO_DEVYATKINA, buildSessionsList(), clientId, -2);
    session->addMessage(info);   // ← через очередь сессии
}

void ServerDevyatkina::broadcastSessions()
{
    std::vector<int> ids;
    {
        std::lock_guard<std::recursive_mutex> lg(m_mx);
        for (const auto& [id, _] : m_clients)
            ids.push_back(id);
    }
    for (int id : ids)
        sendSessionsToClient(id);
}

std::wstring ServerDevyatkina::buildSessionsList()
{
    std::wstring result;
    std::lock_guard<std::recursive_mutex> lg(m_mx);
    for (const auto& [id, _] : m_sessions)
    {
        result += std::to_wstring(id);
        result += L",";
    }
    return result;
}

void ServerDevyatkina::removeClient(int clientId, bool notifyClient)
{
    std::shared_ptr<SessionDevyatkina> session;
    std::shared_ptr<tcp::socket> socket;

    {
        std::lock_guard<std::recursive_mutex> lg(m_mx);
        auto itSession = m_sessions.find(clientId);
        if (itSession != m_sessions.end())
            session = itSession->second;

        auto itSocket = m_clients.find(clientId);
        if (itSocket != m_clients.end())
            socket = itSocket->second;

        m_clients.erase(clientId);
        m_sessions.erase(clientId);
        LocalTransportDevyatkina::eraseSession(clientId);

        // Отпускаем поток-воркер не дожидаясь его завершения
        auto itWorker = m_workers.find(clientId);
        if (itWorker != m_workers.end())
        {
            itWorker->second.detach();
            m_workers.erase(itWorker);
        }
    }

    if (session)
    {
        MessageDevyatkina stop(MT_DISCONNECT_DEVYATKINA, L"", clientId, -2);
        session->addMessage(stop);
        session->stop();
    }

    if (socket && notifyClient)
    {
        try
        {
            SocketTransportDevyatkina transport(socket);
            MessageDevyatkina notify(MT_DISCONNECT_DEVYATKINA, L"", clientId, -2);
            notify.send(transport);
        }
        catch (...) {}
    }

    if (socket)
    {
        boost::system::error_code ec;
        socket->close(ec);
    }

    SafeWrite("server", "client removed", clientId);
    broadcastSessions();
}
