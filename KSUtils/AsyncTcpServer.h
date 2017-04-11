#pragma once

#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\enable_shared_from_this.hpp>
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
typedef boost::shared_ptr<io_service> io_service_ptr;
typedef boost::shared_ptr<ip::tcp::endpoint> endpoint_ptr;
typedef boost::shared_ptr<ip::tcp::acceptor> acceptor_ptr;

template <class T>
class AsyncTcpServer
	: public boost::enable_shared_from_this<T>
	, public boost::noncopyable
{
public:
	AsyncTcpServer(unsigned short port);
	virtual ~AsyncTcpServer();

	virtual void Start();
	virtual void Stop();

	unsigned short GetPort();

protected:
	void HandleAccept(
		boost::shared_ptr<T> svrPtr,
		socket_ptr sockPtr,
		const boost::system::error_code &err);


	virtual void CreateConnection(socket_ptr sock) = 0;
	virtual void Release();

private:
	unsigned short m_Port;
	io_service_ptr m_Service;
	endpoint_ptr m_EndPoint;
	acceptor_ptr m_Acceptor;
};

template <class T>
AsyncTcpServer<T>::AsyncTcpServer(unsigned short port)
	: m_Port(port)
	, m_Service(NULL)
	, m_EndPoint(NULL)
	, m_Acceptor(NULL)
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

	m_Service = boost::make_shared<io_service>();
	m_EndPoint = boost::make_shared<ip::tcp::endpoint>(ip::tcp::v4(), m_Port);
	m_Acceptor = boost::make_shared<ip::tcp::acceptor>(*m_Service, *m_EndPoint);

	socket_ptr sock(new ip::tcp::socket(*m_Service));
	m_Acceptor->async_accept(
		*sock,
		//调用shared_from_this的类之前必须至少有一个share_ptr指向它。
		boost::bind(&AsyncTcpServer::HandleAccept, this, shared_from_this(), sock, _1));

	size_t ret = m_Service->run();
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

	{
		socket_ptr newSock(new ip::tcp::socket(*m_Service));
		m_Acceptor->async_accept(
			*newSock,
			boost::bind(&AsyncTcpServer::HandleAccept, this, shared_from_this(), newSock, _1));
	}
	return;

AcceptError:
	m_Service->stop();
	return;
}

template <class T>
void AsyncTcpServer<T>::Release()
{
	if (m_Acceptor)
	{
		m_Acceptor->close();
		//delete m_Acceptor;
		//m_Acceptor = NULL;
	}

	//if (m_Service)
	//{
	//	m_Service->stop();
	//	delete m_Service;
	//	m_Service = NULL;
	//}

	//if (m_EndPoint)
	//{
	//	delete m_EndPoint;
	//	m_EndPoint = NULL;
	//}
}
