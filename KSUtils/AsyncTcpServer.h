#pragma once

#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\enable_shared_from_this.hpp>
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

template <class T>
class AsyncTcpServer
	: public boost::enable_shared_from_this<T>
	, public boost::noncopyable
{
public:
	AsyncTcpServer(unsigned short port);
	virtual ~AsyncTcpServer();

	void Start();
	void Stop();

	unsigned short GetPort();

protected:
	void HandleAccept(
		boost::shared_ptr<T> svrPtr,
		socket_ptr sockPtr,
		const boost::system::error_code &err);

public://模板不支持protected属性的继承，应是protected成员
	virtual void CreateConnection(socket_ptr sock) = 0;
	virtual void Release();

private:
	unsigned short m_Port;
	io_service *m_pService;
	ip::tcp::endpoint *m_pEndPoint;
	ip::tcp::acceptor *m_pAcceptor;
};

template <class T>
AsyncTcpServer<T>::AsyncTcpServer(unsigned short port)
	: m_Port(port)
	, m_pService(NULL)
	, m_pEndPoint(NULL)
	, m_pAcceptor(NULL)
{
}

template <class T>
AsyncTcpServer<T>::~AsyncTcpServer()
{
}

template <class T>
void AsyncTcpServer<T>::Start()
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

template <class T>
void AsyncTcpServer<T>::Stop()
{
	this->Release();
}

template <class T>
unsigned short AsyncTcpServer<T>::GetPort()
{
	return m_Port;
}

template <class T>
void AsyncTcpServer<T>::HandleAccept(
	boost::shared_ptr<T> svrPtr,
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

template <class T>
void AsyncTcpServer<T>::Release()
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
