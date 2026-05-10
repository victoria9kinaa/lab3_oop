#pragma once

#ifndef SENDER_DEVYATKINA_H
#define SENDER_DEVYATKINA_H

struct Message_Devyatkina;

class Sender_Devyatkina
{
public:
    virtual ~Sender_Devyatkina() = default;
    virtual void send(const Message_Devyatkina& msg) = 0;
};

#endif
