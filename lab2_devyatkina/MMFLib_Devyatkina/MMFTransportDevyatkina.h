#pragma once

#ifndef MMFTRANSPORT_DEVYATKINA_H
#define MMFTRANSPORT_DEVYATKINA_H

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <string>
#include "ITransportDevyatkina.h"

class MMFTransportDevyatkina final : public ITransportDevyatkina
{
public:
    MMFTransportDevyatkina();
    ~MMFTransportDevyatkina() override;

    bool send(const MessageDevyatkina& message) override;
    bool receive(MessageDevyatkina& message) override;

private:
    bool initialize();
    HANDLE openSharedFile() const;

private:
    HANDLE m_mutex;
    std::wstring m_mapName;
    std::wstring m_fileName;
};

#endif
