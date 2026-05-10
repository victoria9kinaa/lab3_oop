#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "../Lab3Devyatkina.TransportDll/SocketTransportDevyatkina.h"
#include "WorkerThreadDevyatkina.h"

class ClientHandlerDevyatkina
{
	SocketTransportDevyatkina m_transport;
	std::mutex m_mx;
	std::vector<std::unique_ptr<WorkerThreadDevyatkina>> m_workers;

	int threadCount() const;
	void sendCount();
	void sendAck();

	void handleStart(const MessageDevyatkina& req);
	bool handleStop();
	void handleData(const MessageDevyatkina& req);

public:
	ClientHandlerDevyatkina(std::shared_ptr<tcp::socket> socket);
	~ClientHandlerDevyatkina();

	void run();
};

