#include "WorkerThreadDevyatkina.h"

#include <fstream>
#include <filesystem>
#include <iostream>

#include "../Lab3Devyatkina.TransportDll/LocalTransportDevyatkina.h"
#include "../Lab3Devyatkina.TransportDll/ProtocolDevyatkina.h"

WorkerThreadDevyatkina::WorkerThreadDevyatkina(int id)
	:m_id(id)
{
	m_session = std::make_shared<SessionDevyatkina>(id);
	LocalTransportDevyatkina::setSession(id, m_session);

	m_running = true;
	m_thread = std::thread(&WorkerThreadDevyatkina::run, this);
}

WorkerThreadDevyatkina::~WorkerThreadDevyatkina()
{
	stop();
	if (m_thread.joinable())
		m_thread.join();
	LocalTransportDevyatkina::eraseSession(m_id);
}

void WorkerThreadDevyatkina::stop()
{
	if (!m_running)
		return;
	m_running = false;
	MessageDevyatkina m(MT_STOP_DEVYATKINA, L"", m_id, 0);
	m_session->addMessage(m);
}

void WorkerThreadDevyatkina::run()
{
	std::wcout << L"=== WORKER " << m_id << L" STARTED ===" << std::endl;
	std::wcout << L"Worker " << m_id << L" is running in directory: " << std::filesystem::current_path().wstring() << std::endl;
	
	LocalTransportDevyatkina tr(m_id);
	while (true)
	{
		MessageDevyatkina m;
		tr.receive(m);

		std::wcout << L"Worker " << m_id << L" received message type: " << m.header.type << std::endl;

		if (m.header.type == MT_STOP_DEVYATKINA)
		{
			std::wcout << L"Worker " << m_id << L" received STOP message, exiting..." << std::endl;
			return;
		}

		if (m.header.type == MT_DATA_DEVYATKINA)
		{
			std::wcout << L"Worker " << m_id << L" processing DATA message: " << m.data << std::endl;
			
			std::wstring filename = std::to_wstring(m_id) + L".txt";
			std::wofstream f(filename, std::ios::app);
			
			// Debug logging
			std::wcout << L"Worker " << m_id << L" attempting to write to file: " << filename << std::endl;
			std::wcout << L"Current working directory: " << std::filesystem::current_path().wstring() << std::endl;
			std::wcout << L"File open status: " << (f.is_open() ? L"SUCCESS" : L"FAILED") << std::endl;
			
			if (f.is_open())
			{
				f << m.data << std::endl;
				f.close();
				std::wcout << L"Worker " << m_id << L" successfully wrote data: " << m.data << std::endl;
			}
			else
			{
				std::wcout << L"Worker " << m_id << L" FAILED to open file for writing!" << std::endl;
			}
		}
	}
}

