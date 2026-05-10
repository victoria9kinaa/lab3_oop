#include "ClientHandlerDevyatkina.h"

#include "../Lab3Devyatkina.TransportDll/ProtocolDevyatkina.h"
#include "../Lab3Devyatkina.TransportDll/SysProgDevyatkina.h"
#include "../Lab3Devyatkina.TransportDll/LocalTransportDevyatkina.h"

static int ParseIntLocalDevyatkina(const std::wstring& s)
{
	try
	{
		return std::stoi(s);
	}
	catch (...)
	{
		return 0;
	}
}

ClientHandlerDevyatkina::ClientHandlerDevyatkina(std::shared_ptr<tcp::socket> socket)
	:m_transport(socket)
{
}

ClientHandlerDevyatkina::~ClientHandlerDevyatkina()
{
	std::lock_guard<std::mutex> lg(m_mx);
	m_workers.clear();
	m_transport.close();
}

int ClientHandlerDevyatkina::threadCount() const
{
	return int(m_workers.size());
}

void ClientHandlerDevyatkina::sendCount()
{
	MessageDevyatkina resp(MT_INFO_DEVYATKINA, std::to_wstring(threadCount()));
	resp.send(m_transport);
}

void ClientHandlerDevyatkina::sendAck()
{
	MessageDevyatkina resp(MT_ACK_DEVYATKINA, L"ok");
	resp.send(m_transport);
}

void ClientHandlerDevyatkina::handleStart(const MessageDevyatkina& req)
{
	const int n = ParseIntLocalDevyatkina(req.data);
	if (n <= 0)
		return;

	for (int i = 0; i < n; ++i)
	{
		const int id = int(m_workers.size()) + 1;
		m_workers.push_back(std::make_unique<WorkerThreadDevyatkina>(id));
		SafeWriteDevyatkina("session", id, "created");
	}
}

bool ClientHandlerDevyatkina::handleStop()
{
	if (m_workers.empty())
		return true;

	const int id = m_workers.back()->id();
	m_workers.pop_back();
	SafeWriteDevyatkina("session", id, "closed");

	return m_workers.empty();
}

void ClientHandlerDevyatkina::handleData(const MessageDevyatkina& req)
{
	if (req.header.to == 0)
	{
		wcout << req.data << endl;
		return;
	}

	if (req.header.to == -1)
	{
		wcout << req.data << endl;
		for (auto& w : m_workers)
		{
			MessageDevyatkina m(MT_DATA_DEVYATKINA, req.data, w->id(), 0);
			LocalTransportDevyatkina().send(m);
		}
		return;
	}

	MessageDevyatkina m(MT_DATA_DEVYATKINA, req.data, req.header.to, 0);
	LocalTransportDevyatkina().send(m);
}

void ClientHandlerDevyatkina::run()
{
	try
	{
		sendCount();		// on connect

		while (m_transport.isConnected())
		{
			MessageDevyatkina req = MessageDevyatkina::receiveMessage(m_transport);

			std::lock_guard<std::mutex> lg(m_mx);
			switch (req.header.type)
			{
				case MT_INFO_DEVYATKINA:
				{
					sendCount();
					break;
				}
				case MT_START_DEVYATKINA:
				{
					handleStart(req);
					sendCount();
					break;
				}
				case MT_STOP_DEVYATKINA:
				{
					const bool empty = handleStop();
					sendCount();
					if (empty)
					{
						m_transport.close();
						return;
					}
					break;
				}
				case MT_DATA_DEVYATKINA:
				{
					handleData(req);
					sendAck();
					break;
				}
				default:
				{
					sendAck();
					break;
				}
			}
		}
	}
	catch (...)
	{
	}
}

