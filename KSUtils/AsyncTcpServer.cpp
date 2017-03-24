#include "AsyncTcpServer.h"

AsyncTcpServer::AsyncTcpServer(unsigned short port)
	: m_Port(port)
	, m_pService(NULL)
	, m_pEndPoint(NULL)
	, m_pAcceptor(NULL)
{
}

AsyncTcpServer::~AsyncTcpServer()
{
}

void AsyncTcpServer::Start()
{
	this->Release();

	m_pService = new io_service;
	m_pEndPoint = new ip::tcp::endpoint(ip::tcp::v4(), m_Port);
	m_pAcceptor = new ip::tcp::acceptor(*m_pService, *m_pEndPoint);

	socket_ptr sock(new ip::tcp::socket(*m_pService));
	m_pAcceptor->async_accept(
		*sock,
		//调用shared_from_this的类之前必须至少有一个share_ptr指向它。
		boost::bind(&AsyncTcpServer::HandleAccept, this, shared_from_this(), sock, _1));

	size_t ret = m_pService->run();
}

void AsyncTcpServer::Stop()
{
	this->Release();
}

unsigned short AsyncTcpServer::GetPort()
{
	return m_Port;
}

void AsyncTcpServer::HandleAccept(
	AsyncTcpServerPtr svrPtr,
	socket_ptr sockPtr,
	const boost::system::error_code &err)
{
	if (err)
		goto AcceptError;

	this->CreateConnection(sockPtr);

AcceptError:
	socket_ptr newSock(new ip::tcp::socket(*m_pService));
	m_pAcceptor->async_accept(
		*newSock,
		boost::bind(&AsyncTcpServer::HandleAccept, this, shared_from_this(), newSock, _1));
}

void AsyncTcpServer::Release()
{
	if (m_pAcceptor)
	{
		m_pAcceptor->close();
		delete m_pAcceptor;
		m_pAcceptor = NULL;
	}

	if (m_pService)
	{
		m_pService->stop();
		delete m_pService;
		m_pService = NULL;
	}

	if (m_pEndPoint)
	{
		delete m_pEndPoint;
		m_pEndPoint = NULL;
	}
}

//template class AsyncTcpServer<int>;
//template class AsyncTcpServer<int>;