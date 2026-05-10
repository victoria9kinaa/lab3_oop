#pragma once

#ifndef HEADER_DEVYATKINA_H
#define HEADER_DEVYATKINA_H

struct HeaderDevyatkina
{
    int type;      // command type or ACK
    int size;      // payload bytes
    int targetId;  // -1 all, 0 main, >0 worker id
    int param;     // extra command parameter (e.g. threads count)
    int status;    // ACK status from console
};

#endif
