#pragma once

#ifndef WORKER_SESSION_DEVYATKINA_H
#define WORKER_SESSION_DEVYATKINA_H

#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "MessageEndpoints_Devyatkina.h"

class WorkerSessionDevyatkina : public Sender_Devyatkina, public Receiver_Devyatkina
{
public:
    explicit WorkerSessionDevyatkina(int id);
    ~WorkerSessionDevyatkina();

    void start();
    void stop();
    void post(const std::wstring& text);
    void send(const ThreadMessageDevyatkina& msg) override;
    bool receive(ThreadMessageDevyatkina& msg) override;
    int id() const;

private:
    void run();

private:
    int m_id;
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<ThreadMessageDevyatkina> m_queue;
    bool m_stopping;
};

#endif
