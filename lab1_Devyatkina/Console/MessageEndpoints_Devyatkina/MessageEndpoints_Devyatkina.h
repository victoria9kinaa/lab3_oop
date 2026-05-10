#pragma once

#ifndef MESSAGE_ENDPOINTS_DEVYATKINA_H
#define MESSAGE_ENDPOINTS_DEVYATKINA_H

struct Message_Devyatkina;

class Sender_Devyatkina
{
public:
    virtual ~Sender_Devyatkina() = default;
    virtual void send(const Message_Devyatkina& msg) = 0;
};

class Receiver_Devyatkina
{
public:
    virtual ~Receiver_Devyatkina() = default;
    virtual void receive(Message_Devyatkina& msg) = 0;
};

#endif
