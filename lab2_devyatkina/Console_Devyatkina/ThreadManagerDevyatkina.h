#pragma once

#ifndef THREAD_MANAGER_DEVYATKINA_H
#define THREAD_MANAGER_DEVYATKINA_H

#include <memory>
#include <string>
#include <vector>

#include "WorkerSessionDevyatkina.h"

class ThreadManagerDevyatkina
{
public:
    ThreadManagerDevyatkina();
    ~ThreadManagerDevyatkina();

    void create(int count);
    void stopLast();
    void stopAll();
    int count() const;

    void sendTo(int targetId, const std::wstring& text);
    void sendToAll(const std::wstring& text);

private:
    std::vector<std::unique_ptr<WorkerSessionDevyatkina>> m_sessions;
    int m_nextId;
};

#endif
