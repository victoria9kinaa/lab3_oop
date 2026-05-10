#pragma once

#ifndef IMESSAGE_STRATEGY_DEVYATKINA_H
#define IMESSAGE_STRATEGY_DEVYATKINA_H

#include <string>

class ITransport_Devyatkina;

enum class MessageType_Devyatkina
{
    Close,
    SendText,
};

struct Message_Devyatkina
{
    MessageType_Devyatkina type {};
    std::wstring payload;

    void send(ITransport_Devyatkina& transport, class Session_Devyatkina& session);
    void receive(ITransport_Devyatkina& transport, class Session_Devyatkina& session);
};

#endif
