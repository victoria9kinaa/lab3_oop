#include "IMessageStrategy_Devyatkina.h"
#include "../Transport_Devyatkina/ITransport_Devyatkina.h"
#include "../Session_Devyatkina/Session_Devyatkina.h"

void Message_Devyatkina::send(ITransport_Devyatkina& transport, Session_Devyatkina& session)
{
    transport.send(session, *this);
}

void Message_Devyatkina::receive(ITransport_Devyatkina& transport, Session_Devyatkina& session)
{
    transport.receive(session, *this);
}
