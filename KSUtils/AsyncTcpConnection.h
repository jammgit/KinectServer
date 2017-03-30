#pragma once

#include "ShareData.h"
#include "ShareFrame.h"
#include "../IKSKinectDataCapture/Basic264Frame.h"
#include "../IKSKinectDataCapture/EncodedColorFrame.h"
#include "../IKSKinectDataCapture/EncodedDepthFrame.h"
#include "SocketAddress.h"
#include <mutex>
#include <list>
#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\enable_shared_from_this.hpp>

using namespace boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

class AsyncTcpConnection;
typedef boost::shared_ptr<AsyncTcpConnection> AsyncTcpConnectionPtr;

class Middle264Frame;
typedef boost::shared_ptr<Middle264Frame> Middle264FramePtr;

class SkeletonFrame;
typedef boost::shared_ptr<SkeletonFrame> SkeletonFramePtr;

class AsyncTcpConnection
	: public boost::enable_shared_from_this<AsyncTcpConnection>
	, public boost::noncopyable
{
public:
	AsyncTcpConnection(socket_ptr sock);
	~AsyncTcpConnection();

	size_t GetDataQueueSize();
	Address GetAddrPort();
	std::string GetAddrStr();

	virtual void DoRead();

	//发送协议
	virtual void SendShareFrame(ShareFrame frame) {};

	////发送Kinect数据
	//virtual void Open() {};
	//virtual void Close() {};
	//virtual void Send264Frame(Middle264FramePtr frame) {};
	//virtual void SendSkeletonFrame(SkeletonFramePtr frame) {};

protected:
	void HandleRead(
		AsyncTcpConnectionPtr connPtr,
		ShareData data,
		const boost::system::error_code& err,
		std::size_t rbytes);

	void HandleWrite(
		AsyncTcpConnectionPtr connPtr,
		ShareData data,
		const boost::system::error_code& err,
		std::size_t wbytes);

	////发送二进制流
	//virtual void SendShareData(const ShareData& data);

protected:
	virtual void TryParse(const ShareData& data) {};
	virtual void SendShareData(const ShareData& data);
	virtual void Release();

protected:
	const int READ_BUF_LEN = 1024;
	socket_ptr m_Socket;

	//防止连续呼叫回调导致数据乱序
	bool m_bFirstTimeRead;
	std::mutex m_callingMutex;
	bool m_bAsyncWriteCalling;

	std::mutex m_dataListMutex;
	std::list<ShareData> m_dataList;
	Address m_addr;
};
