#pragma once

#ifndef ITRANSPORT_DEVYATKINA_H
#define ITRANSPORT_DEVYATKINA_H

#include "MessageDevyatkina.h"

class ITransportDevyatkina
{
public:
    virtual ~ITransportDevyatkina() = default;
    virtual bool send(const MessageDevyatkina& message) = 0;
    virtual bool receive(MessageDevyatkina& message) = 0;
};

#endif
