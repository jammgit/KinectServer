#pragma once

#include "ShareData.h"
#include "ShareFrame.h"
#include <mutex>
#include <list>
#include <boost\asio.hpp>
#include <boost\enable_shared_from_this.hpp>

using namespace boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
class AsyncTcpConnection;
typedef boost::shared_ptr<AsyncTcpConnection> AsyncTcpConnectionPtr;

class AsyncTcpConnection
	: public boost::enable_shared_from_this<AsyncTcpConnection>
	, public boost::noncopyable
{
public:
	AsyncTcpConnection(socket_ptr sock);
	~AsyncTcpConnection();

	virtual void DoRead();

	virtual void SendShareFrame(ShareFrame frame) = 0;

protected:
	void HandleRead(
		ShareData data,
		const boost::system::error_code& err,
		std::size_t rbytes);

	void HandleWrite(
		ShareData data,
		const boost::system::error_code& err,
		std::size_t wbytes);

protected:
	virtual void TryParse(const ShareData& data) = 0;
	virtual void SendShareData(const ShareData& data);

private:
	const int READ_BUF_LEN = 1024;
	socket_ptr m_Socket;

	//防止连续呼叫回调导致数据乱序
	bool m_bFirstTimeRead;
	std::mutex m_callingMutex;
	bool m_bAsyncWriteCalling;

	std::mutex m_dataListMutex;
	std::list<ShareData> m_dataList;
};
