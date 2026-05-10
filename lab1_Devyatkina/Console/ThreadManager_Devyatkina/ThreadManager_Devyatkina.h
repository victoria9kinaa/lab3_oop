#pragma once
// ============================================================
//  ThreadManager_Devyatkina.h
//  Менеджер сессий: создание, остановка, хранение
// ============================================================

#ifndef THREADMANAGER_DEVYATKINA_H
#define THREADMANAGER_DEVYATKINA_H

#include <Windows.h>
#include <vector>
#include <memory>
#include "../Session_Devyatkina/Session_Devyatkina.h"
#include "../Transport_Devyatkina/ITransport_Devyatkina.h"

// ============================================================
class ThreadManager_Devyatkina
{
public:
    ThreadManager_Devyatkina();
    ~ThreadManager_Devyatkina();

    // Создать N новых сессий (потоков)
    void CreateSessions(int count);

    // Остановить последнюю сессию (LIFO)
    // Возвращает false, если сессий нет
    bool StopLastSession();

    // Остановить все сессии
    void StopAllSessions();

    // Отправить текстовое сообщение последней сессии
    void SendToLast(const std::wstring& text);

    // Количество живых сессий
    int  Count() const;

private:
    // Убрать завершённые сессии из вектора
    void Cleanup();

private:
    std::shared_ptr<ITransport_Devyatkina> m_transport;
    std::vector<std::unique_ptr<Session_Devyatkina>> m_sessions;
    int m_nextId;
};

#endif // THREADMANAGER_DEVYATKINA_H
