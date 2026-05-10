#pragma once

#ifndef ITRANSPORT_DEVYATKINA_H
#define ITRANSPORT_DEVYATKINA_H

struct Message_Devyatkina;
class Session_Devyatkina;

class ITransport_Devyatkina
{
public:
    virtual ~ITransport_Devyatkina() = default;
    virtual void send(Session_Devyatkina& session, Message_Devyatkina& message) = 0;
    virtual void receive(Session_Devyatkina& session, Message_Devyatkina& message) = 0;
};

class LocalTransport_Devyatkina final : public ITransport_Devyatkina
{
public:
    void send(Session_Devyatkina& session, Message_Devyatkina& message) override;
    void receive(Session_Devyatkina& session, Message_Devyatkina& message) override;
};

class MMFTransport_Devyatkina final : public ITransport_Devyatkina
{
public:
    void send(Session_Devyatkina& session, Message_Devyatkina& message) override;
    void receive(Session_Devyatkina& session, Message_Devyatkina& message) override;
};

class SocketTransport_Devyatkina final : public ITransport_Devyatkina
{
public:
    void send(Session_Devyatkina& session, Message_Devyatkina& message) override;
    void receive(Session_Devyatkina& session, Message_Devyatkina& message) override;
};

class BrokerTransport_Devyatkina final : public ITransport_Devyatkina
{
public:
    void send(Session_Devyatkina& session, Message_Devyatkina& message) override;
    void receive(Session_Devyatkina& session, Message_Devyatkina& message) override;
};

#endif
