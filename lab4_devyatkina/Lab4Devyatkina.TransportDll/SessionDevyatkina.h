#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "MessageDevyatkina.h"
#include "TransportApiDevyatkina.h"

class LAB4DEVYATKINA_TRANSPORTDLL_API SessionDevyatkina
{
	std::queue<MessageDevyatkina> m_messages;
	std::mutex m_mx;
	std::condition_variable m_ev;
	std::chrono::steady_clock::time_point m_lastRequest;

public:
	int sessionID = 0;

	SessionDevyatkina(int sessionID);

	void addMessage(MessageDevyatkina& m);
	void addMessage(int type, const std::wstring& data = L"", int to = 0, int from = 0);
	bool getMessage(MessageDevyatkina& m);

	void updateActivity();
	bool isTimedOut(std::chrono::milliseconds timeout) const;
};

