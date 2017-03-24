#pragma once

#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\enable_shared_from_this.hpp>
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

class AsyncTcpServer;
typedef boost::shared_ptr<AsyncTcpServer> AsyncTcpServerPtr;

class KSService;
class KSKinectDataServer;

class AsyncTcpServer
	: public boost::enable_shared_from_this<AsyncTcpServer>
	, public boost::noncopyable
{
public:
	AsyncTcpServer(unsigned short port);
	~AsyncTcpServer();

	void Start();
	void Stop();

	unsigned short GetPort();

protected:
	void HandleAccept(
		AsyncTcpServerPtr svrPtr,
		socket_ptr sockPtr,
		const boost::system::error_code &err);

	virtual void CreateConnection(socket_ptr sock) = 0;

	void Release();

private:
	unsigned short m_Port;
	io_service *m_pService;
	ip::tcp::endpoint *m_pEndPoint;
	ip::tcp::acceptor *m_pAcceptor;
};
