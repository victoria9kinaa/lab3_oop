#pragma once

#ifndef MESSAGE_DEVYATKINA_H
#define MESSAGE_DEVYATKINA_H

#include <string>
#include "HeaderDevyatkina.h"

enum class MessageTypeDevyatkina
{
    None = 0,
    StartThreads = 1,
    StopLastThread = 2,
    SendText = 3,
    Shutdown = 4,
    Ack = 5
};

struct MessageDevyatkina
{
    HeaderDevyatkina header {};
    std::wstring data;
};

#endif
