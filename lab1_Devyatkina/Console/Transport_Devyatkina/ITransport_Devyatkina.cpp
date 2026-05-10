#include "ITransport_Devyatkina.h"

#include "../Session_Devyatkina/Session_Devyatkina.h"

void LocalTransport_Devyatkina::send(Session_Devyatkina& session, Message_Devyatkina& message)
{
    session.EnqueueMessage(message);
}

void LocalTransport_Devyatkina::receive(Session_Devyatkina& session, Message_Devyatkina& message)
{
    session.DequeueMessage(message);
}

void MMFTransport_Devyatkina::send(Session_Devyatkina& session, Message_Devyatkina& message)
{
    // MMF transport can be added later; local queue keeps behavior stable.
    session.EnqueueMessage(message);
}

void MMFTransport_Devyatkina::receive(Session_Devyatkina& session, Message_Devyatkina& message)
{
    session.DequeueMessage(message);
}

void SocketTransport_Devyatkina::send(Session_Devyatkina& session, Message_Devyatkina& message)
{
    // Socket transport can be added later; local queue keeps behavior stable.
    session.EnqueueMessage(message);
}

void SocketTransport_Devyatkina::receive(Session_Devyatkina& session, Message_Devyatkina& message)
{
    session.DequeueMessage(message);
}

void BrokerTransport_Devyatkina::send(Session_Devyatkina& session, Message_Devyatkina& message)
{
    // Broker transport can be added later; local queue keeps behavior stable.
    session.EnqueueMessage(message);
}

void BrokerTransport_Devyatkina::receive(Session_Devyatkina& session, Message_Devyatkina& message)
{
    session.DequeueMessage(message);
}
