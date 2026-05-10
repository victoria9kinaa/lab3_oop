#include "ServerDevyatkina.h"
#include <fstream>

std::vector<jthread> ServerDevyatkina::m_threads;
std::map<int, SessionDevyatkina*> ServerDevyatkina::m_sessions;
std::mutex ServerDevyatkina::m_mx;
std::shared_ptr<tcp::acceptor> ServerDevyatkina::m_acceptor;
int ServerDevyatkina::m_nextThreadId = 1;

ServerDevyatkina::ServerDevyatkina()
{
	try
	{
		m_acceptor = SocketTransportDevyatkina::listen(SocketTransportDevyatkina::SERVER_PORT);
		if (!m_acceptor)
		{
			SafeWrite("server", "failed to create acceptor");
			return;
		}
		SafeWrite("server", "acceptor created successfully");
	}
	catch (const std::exception& e)
	{
		SafeWrite("server", "acceptor creation failed:", e.what());
	}
	catch (...)
	{
		SafeWrite("server", "acceptor creation failed: unknown error");
	}
}

void ServerDevyatkina::run()
{
	SafeWrite("server", "start");

	while (true)
	{
		try
		{
			auto socket = SocketTransportDevyatkina::accept(m_acceptor);
			if (!socket)
				continue;

			thread([socket]()
			{
				try
				{
					SocketTransportDevyatkina clientTransport(socket);
					
					// Отправляем начальное количество потоков
					MessageDevyatkina resp(MT_INFO_DEVYATKINA, std::to_wstring(m_threads.size()));
					resp.send(clientTransport);

					while (clientTransport.isConnected())
					{
						try
						{
							MessageDevyatkina msg = MessageDevyatkina::receiveMessage(clientTransport);
							handleClientMessage(msg, clientTransport);
						}
						catch (const std::exception& e)
						{
							SafeWrite("server", "client message error:", e.what());
							break;
						}
						catch (...)
						{
							SafeWrite("server", "client message error: unknown");
							break;
						}
					}
				}
				catch (const std::exception& e)
				{
					SafeWrite("server", "client handler error:", e.what());
				}
				catch (...)
				{
					SafeWrite("server", "client handler error: unknown");
				}
			}).detach();
		}
		catch (const std::exception& e)
		{
			SafeWrite("server", "accept error:", e.what());
			break;
		}
		catch (...)
		{
			SafeWrite("server", "accept error: unknown");
			break;
		}
	}
}

void ServerDevyatkina::handleClientMessage(const MessageDevyatkina& msg, SocketTransportDevyatkina& clientTransport)
{
	switch (msg.header.type)
	{
	case MT_START_DEVYATKINA:
	{
		try
		{
			const int n = std::stoi(msg.data);
			std::lock_guard<std::mutex> lg(m_mx);
			for (int i = 0; i < n; ++i)
			{
				const int threadId = m_nextThreadId++;
				m_threads.emplace_back(&ServerDevyatkina::worker, threadId);
				SafeWrite("session", threadId, "created");
			}
		}
		catch (const std::exception& e)
		{
			SafeWrite("server", "start error:", e.what());
		}
		catch (...)
		{
			SafeWrite("server", "start error: unknown");
		}
		break;
	}
	case MT_STOP_DEVYATKINA:
	{
		try
		{
			if (!m_threads.empty())
			{
				// Send stop message to last thread through session
				std::lock_guard<std::mutex> lg(m_mx);
				int threadId = 0;
				if (!m_sessions.empty())
				{
					// Get the highest thread ID (last created thread)
					threadId = m_sessions.rbegin()->first;
				}
				auto it = m_sessions.find(threadId);
				if (it != m_sessions.end() && it->second)
				{
					MessageDevyatkina stopMsg(MT_STOP_DEVYATKINA, L"", threadId, 0);
					it->second->addMessage(stopMsg);
					SafeWrite("server", "sent stop to thread", threadId);
				}
				
				// Detach thread to prevent UI blocking
				if (m_threads.back().joinable())
				{
					m_threads.back().detach();
					SafeWrite("server", "thread", threadId, "detached");
				}
				
				m_threads.pop_back();
				
				// Clean up session
				auto sessionIt = m_sessions.find(threadId);
				if (sessionIt != m_sessions.end())
				{
					m_sessions.erase(sessionIt);
					SafeWrite("session", threadId, "deleted");
				}
				
				SafeWrite("session", threadId, "closed");
				
				// Reset thread ID counter when all threads are stopped
				if (m_sessions.empty())
				{
					m_nextThreadId = 1;
					SafeWrite("server", "thread counter reset to 1");
				}
			}
		}
		catch (const std::exception& e)
		{
			SafeWrite("server", "stop error:", e.what());
		}
		catch (...)
		{
			SafeWrite("server", "stop error: unknown");
		}
		break;
	}
	case MT_DATA_DEVYATKINA:
	{
		try
		{
			// Send message directly to sessions (working approach)
			std::lock_guard<std::mutex> lg(m_mx);
			
			if (msg.header.to == -1) // All threads
			{
				for (auto& [threadId, session] : m_sessions)
				{
					if (session)
					{
						// Send message with to=-1 to all threads
						session->addMessage(const_cast<MessageDevyatkina&>(msg));
						SafeWrite("server", "sent to all threads:", msg.data);
					}
				}
			}
			else if (msg.header.to == 0) // Main thread
			{
				std::wcout << "Main thread received: " << msg.data << std::endl;
				SafeWrite("server", "sent to main thread:", msg.data);
			}
			else // Specific thread
			{
				auto it = m_sessions.find(msg.header.to);
				if (it != m_sessions.end() && it->second)
				{
					it->second->addMessage(const_cast<MessageDevyatkina&>(msg));
					SafeWrite("server", "sent to thread", msg.header.to, ":", msg.data);
				}
				else
				{
					SafeWrite("server", "thread", msg.header.to, "not found - message ignored");
					// Send error response to client
					MessageDevyatkina errorResp(MT_INFO_DEVYATKINA, L"Thread not found", 0, 0);
					errorResp.send(clientTransport);
				}
			}
		}
		catch (const std::exception& e)
		{
			SafeWrite("server", "data error:", e.what());
		}
		catch (...)
		{
			SafeWrite("server", "data error: unknown");
		}
		break;
	}
	}

	// Отправляем ответ клиенту
	MessageDevyatkina resp(MT_INFO_DEVYATKINA, std::to_wstring(m_threads.size()));
	if (msg.header.type == MT_DATA_DEVYATKINA)
	{
		// Для сообщений данных отправляем ACK
		resp.header.type = MT_ACK_DEVYATKINA;
		resp.data = L"Message received";
	}
	resp.send(clientTransport);
}

void ServerDevyatkina::worker(int threadId)
{
	auto session = new SessionDevyatkina(threadId);
	
	{
		std::lock_guard<std::mutex> lg(m_mx);
		m_sessions[threadId] = session;
	}

	SafeWrite("session", threadId, "created");

	while (true)
	{
		try
		{
			MessageDevyatkina msg;
			SafeWrite("worker", threadId, "waiting for message...");
			session->getMessage(msg);
			SafeWrite("worker", threadId, "received message type:", msg.header.type, "to:", msg.header.to);

			if (msg.header.type == MT_STOP_DEVYATKINA && msg.header.to == threadId)
			{
				SafeWrite("worker", threadId, "stopped");
				break;
			}

			if (msg.header.type == MT_DATA_DEVYATKINA)
			{
				if (msg.header.to == threadId)
				{
					// Worker thread - write to file
					SafeWrite("session", threadId, "data", msg.data);
					
					// Write to file
					std::wofstream f(std::to_wstring(threadId) + L".txt", std::ios::app);
					if (f.is_open())
					{
						f << msg.data << std::endl;
						f.close();
						SafeWrite("session", threadId, "wrote to file", msg.data);
					}
					else
					{
						SafeWrite("session", threadId, "failed to open file");
					}
				}
				else if (msg.header.to == -1)
				{
					// All threads - write to file
					SafeWrite("session", threadId, "all threads data", msg.data);
					
					// Write to file
					std::wofstream f(std::to_wstring(threadId) + L".txt", std::ios::app);
					if (f.is_open())
					{
						f << msg.data << std::endl;
						f.close();
						SafeWrite("session", threadId, "wrote all threads data", msg.data);
					}
					else
					{
						SafeWrite("session", threadId, "failed to open file");
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			SafeWrite("worker", threadId, "error:", e.what());
			break;
		}
		catch (...)
		{
			SafeWrite("worker", threadId, "error: unknown");
			break;
		}
	}

	{
		std::lock_guard<std::mutex> lg(m_mx);
		m_sessions.erase(threadId);
		delete session;
		SafeWrite("session", threadId, "closed");
	}
}

