#pragma once

#ifndef MESSAGE_ENDPOINTS_DEVYATKINA_H
#define MESSAGE_ENDPOINTS_DEVYATKINA_H

#include <string>

struct ThreadMessageDevyatkina
{
    std::wstring payload;
};

class Sender_Devyatkina
{
public:
    virtual ~Sender_Devyatkina() = default;
    virtual void send(const ThreadMessageDevyatkina& msg) = 0;
};

class Receiver_Devyatkina
{
public:
    virtual ~Receiver_Devyatkina() = default;
    virtual bool receive(ThreadMessageDevyatkina& msg) = 0;
};

#endif
