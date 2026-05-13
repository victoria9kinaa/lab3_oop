#pragma once

// Define NOMINMAX to prevent min/max macro conflicts
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

// Include Winsock before Windows to avoid winsock.h conflicts
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "ITransportDevyatkina.h"
#include "MessageDevyatkina.h"
#include "TransportApiDevyatkina.h"
#include <memory>
#include <mutex>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class LAB4DEVYATKINA_TRANSPORTDLL_API SocketTransportDevyatkina : public ITransportDevyatkina
{
public:
	static inline const char* SERVER_IP = "127.0.0.1";
	static inline const int SERVER_PORT = 12345;

	SocketTransportDevyatkina();
	SocketTransportDevyatkina(std::shared_ptr<tcp::socket> socket);
	~SocketTransportDevyatkina() = default;

	bool connect();
	bool isConnected() const;
	void close();

	virtual void send(MessageDevyatkina& m) const override;
	virtual void receive(MessageDevyatkina& m) const override;

	static std::shared_ptr<tcp::acceptor> listen(int port = SERVER_PORT);
	static std::shared_ptr<tcp::socket> accept(std::shared_ptr<tcp::acceptor> acc);

private:
	static inline std::mutex mx;
	static inline boost::asio::io_context server_io;

	boost::asio::io_context client_io;
	std::shared_ptr<tcp::socket> m_socket;

	static void sendSocket(tcp::socket& socket, MessageDevyatkina& msg);
	static MessageDevyatkina receiveSocket(tcp::socket& socket);
};

