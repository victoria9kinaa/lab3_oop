#pragma once

#ifndef RECEIVER_DEVYATKINA_H
#define RECEIVER_DEVYATKINA_H

struct Message_Devyatkina;

class Receiver_Devyatkina
{
public:
    virtual ~Receiver_Devyatkina() = default;
    virtual void receive(Message_Devyatkina& msg) = 0;
};

#endif
