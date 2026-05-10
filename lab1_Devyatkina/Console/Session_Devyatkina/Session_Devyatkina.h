#pragma once
// Session_Devyatkina.h
// Класс сессии: очередь сообщений + поток + стратегия обработки

#ifndef SESSION_DEVYATKINA_H
#define SESSION_DEVYATKINA_H

#include <Windows.h>
#include <queue>
#include <memory>
#include "../IMessageStrategy_Devyatkina/IMessageStrategy_Devyatkina.h"
#include "../Transport_Devyatkina/ITransport_Devyatkina.h"
#include "../MessageEndpoints_Devyatkina/MessageEndpoints_Devyatkina.h"

class Session_Devyatkina : public Sender_Devyatkina, public Receiver_Devyatkina
{
public:
    Session_Devyatkina(int id, std::shared_ptr<ITransport_Devyatkina> transport);
    ~Session_Devyatkina();

    void   send(const Message_Devyatkina& msg) override;
    void   receive(Message_Devyatkina& msg) override;
    HANDLE Start();
    void   PostMessage(const Message_Devyatkina& msg);
    void   EnqueueMessage(const Message_Devyatkina& msg);
    void   DequeueMessage(Message_Devyatkina& msg);

    int    GetId()           const { return m_id; }
    HANDLE GetThreadHandle() const { return m_hThread; }
    bool   IsDone()          const { return m_done; }

private:
    static DWORD WINAPI ThreadProc(LPVOID param);
    void Run();

private:
    int                             m_id;
    HANDLE                          m_hThread;
    HANDLE                          m_hEvent;
    CRITICAL_SECTION                m_cs;
    std::queue<Message_Devyatkina>  m_queue;
    volatile bool                   m_done;
    std::shared_ptr<ITransport_Devyatkina> m_transport;
};

#endif
