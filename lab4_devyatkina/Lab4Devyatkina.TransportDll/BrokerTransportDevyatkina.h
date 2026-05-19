#pragma once

#include "ITransportDevyatkina.h"
#include "LocalTransportDevyatkina.h"
#include "SessionDevyatkina.h"
#include <mutex>
#include <stdexcept>

class BrokerTransportDevyatkina : public ITransportDevyatkina
{
public:
	BrokerTransportDevyatkina() = default;

	virtual void send(MessageDevyatkina& m) const override;
	virtual void receive(MessageDevyatkina& m) const override;
};

// Inline implementations to avoid linker issues when using across projects
inline void BrokerTransportDevyatkina::send(MessageDevyatkina& m) const
{
    std::lock_guard<std::mutex> lg(LocalTransportDevyatkina::mx);

    if (m.header.to == -1) // BROADCAST (Всем клиентам)
    {
        for (auto& [id, session] : LocalTransportDevyatkina::sessions)
        {
            // Не отправляем самому себе (если нужно исключить отправителя)
            if (id != m.header.from)
            {
                MessageDevyatkina copy = m;
                copy.header.to = id; // Устанавливаем целевой ID
                session->addMessage(copy);
            }
        }
    }
    else // UNICAST (Конкретному клиенту)
    {
        auto it = LocalTransportDevyatkina::sessions.find(m.header.to);
        if (it != LocalTransportDevyatkina::sessions.end())
        {
            it->second->addMessage(m);
        }
    }
}

inline void BrokerTransportDevyatkina::receive(MessageDevyatkina& m) const
{
    // Метод не используется в архитектуре сервера, так как 
    // сессии забирают сообщения напрямую через LocalTransport.
    throw std::runtime_error("BrokerTransport::receive is not implemented");
}

