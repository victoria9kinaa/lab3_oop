#include "SocketTransportDevyatkina.h"

SocketTransportDevyatkina::SocketTransportDevyatkina()
{
	m_socket = std::make_shared<tcp::socket>(client_io);
}

SocketTransportDevyatkina::SocketTransportDevyatkina(std::shared_ptr<tcp::socket> socket)
	:m_socket(socket)
{
}

bool SocketTransportDevyatkina::connect()
{
	try
	{
		tcp::resolver resolver(client_io);
		auto endpoints = resolver.resolve(SERVER_IP, std::to_string(SERVER_PORT));
		boost::asio::connect(*m_socket, endpoints);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool SocketTransportDevyatkina::isConnected() const
{
	return m_socket && m_socket->is_open();
}

void SocketTransportDevyatkina::close()
{
	if (!m_socket)
		return;
	boost::system::error_code ec;
	m_socket->shutdown(tcp::socket::shutdown_both, ec);
	m_socket->close(ec);
}

void SocketTransportDevyatkina::sendSocket(tcp::socket& socket, MessageDevyatkina& msg)
{
	boost::asio::write(socket, boost::asio::buffer(&msg.header, sizeof(HeaderDevyatkina)));
	if (msg.header.size > 0)
	{
		boost::asio::write(socket, boost::asio::buffer(msg.data.data(), msg.header.size));
	}
}

MessageDevyatkina SocketTransportDevyatkina::receiveSocket(tcp::socket& socket)
{
	MessageDevyatkina msg;
	boost::asio::read(socket, boost::asio::buffer(&msg.header, sizeof(HeaderDevyatkina)));
	msg.data.clear();
	if (msg.header.size > 0)
	{
		std::wstring buff;
		buff.resize(msg.header.size / sizeof(wchar_t));
		boost::asio::read(socket, boost::asio::buffer(buff.data(), msg.header.size));
		msg.data = std::move(buff);
	}
	return msg;
}

void SocketTransportDevyatkina::send(MessageDevyatkina& m) const
{
	if (!m_socket)
		return;
	sendSocket(*m_socket, m);
}

void SocketTransportDevyatkina::receive(MessageDevyatkina& m) const
{
	if (!m_socket)
		return;
	m = receiveSocket(*m_socket);
}

std::shared_ptr<tcp::acceptor> SocketTransportDevyatkina::listen(int port)
{
	std::lock_guard<std::mutex> lg(mx);
	auto acc = std::make_shared<tcp::acceptor>(server_io, tcp::endpoint(tcp::v4(), (unsigned short)port));
	return acc;
}

std::shared_ptr<tcp::socket> SocketTransportDevyatkina::accept(std::shared_ptr<tcp::acceptor> acc)
{
	if (!acc)
		return nullptr;
	auto socket = std::make_shared<tcp::socket>(server_io);
	acc->accept(*socket);
	return socket;
}

