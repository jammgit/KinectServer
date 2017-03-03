#pragma once

#include <boost\asio.hpp>
#include <boost\enable_shared_from_this.hpp>
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
class AsyncTcpServer;
typedef boost::shared_ptr<AsyncTcpServer> AsyncTcpServerPtr;

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
		socket_ptr sockPtr,
		const boost::system::error_code &err);

	virtual void CreateConnection(socket_ptr sock) = 0;

private:
	unsigned short m_Port;
	io_service m_Service;
	ip::tcp::endpoint m_EndPoint;
	ip::tcp::acceptor m_Acceptor;
};
