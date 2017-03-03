#include "AsyncTcpServer.h"
#include <boost\bind.hpp>

AsyncTcpServer::AsyncTcpServer(unsigned short port)
	: m_Port(port)
	, m_EndPoint(ip::tcp::v4(), m_Port)
	, m_Acceptor(m_Service, m_EndPoint)
{
}

AsyncTcpServer::~AsyncTcpServer()
{
}

void AsyncTcpServer::Start()
{
	socket_ptr sock(new ip::tcp::socket(m_Service));

	m_Acceptor.async_accept(
		*sock,
		boost::bind(&AsyncTcpServer::HandleAccept, shared_from_this(), sock, _1));

	m_Service.run();
}

void AsyncTcpServer::Stop()
{
	m_Service.stop();
}

unsigned short AsyncTcpServer::GetPort()
{
	return m_Port;
}

void AsyncTcpServer::HandleAccept(
	socket_ptr sockPtr,
	const boost::system::error_code &err)
{
	if (err)
		goto AcceptError;

	this->CreateConnection(sockPtr);

AcceptError:
	socket_ptr newSock(new ip::tcp::socket(m_Service));
	m_Acceptor.async_accept(
		*newSock,
		boost::bind(&AsyncTcpServer::HandleAccept, shared_from_this(), newSock, _1));
}