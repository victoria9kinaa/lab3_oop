#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "../Lab3Devyatkina.TransportDll/SessionDevyatkina.h"

class WorkerThreadDevyatkina
{
	int m_id = 0;
	std::shared_ptr<SessionDevyatkina> m_session;
	std::thread m_thread;
	std::atomic<bool> m_running = false;

	void run();

public:
	WorkerThreadDevyatkina(int id);
	~WorkerThreadDevyatkina();

	int id() const { return m_id; }
	void stop();
};

