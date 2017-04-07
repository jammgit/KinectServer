#pragma once

#include "ShareData.h"
#include "ShareFrame.h"
#include "SocketAddress.h"
#include <mutex>
#include <list>
#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\enable_shared_from_this.hpp>

using namespace boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

template <class T>
class AsyncTcpConnection
	: public boost::enable_shared_from_this<T>
	, public boost::noncopyable
{
public:
	AsyncTcpConnection(socket_ptr sock);
	virtual ~AsyncTcpConnection();

	size_t GetDataQueueSize();
	Address GetAddrPort();
	std::string GetAddrStr();

	virtual void DoRead();

	//发送协议
	virtual void SendShareFrame(ShareFrame frame) {};

protected:
	void HandleRead(
		boost::shared_ptr<T> connPtr,
		ShareData data,
		const boost::system::error_code& err,
		std::size_t rbytes);

	void HandleWrite(
		boost::shared_ptr<T> connPtr,
		ShareData data,
		const boost::system::error_code& err,
		std::size_t wbytes);

	void SendShareData(const ShareData& data);

protected: 
	virtual void TryParse(const ShareData& data) {};
	virtual void KeepAliveHandler(const boost::system::error_code &) {};
	virtual void Release();
protected:
	static void KeepAliveCallBack(
		const boost::system::error_code &, boost::shared_ptr<T> );

protected:
	const int READ_BUF_LEN = 1024;
	const int KEEP_ALIVE_MS = 30000;

	socket_ptr m_Socket;
	boost::asio::deadline_timer m_tPing;
	
	//防止连续呼叫回调导致数据乱序
	bool m_bFirstTimeRead;
	std::mutex m_callingMutex;
	bool m_bAsyncWriteCalling;

	std::mutex m_dataListMutex;
	std::list<ShareData> m_dataList;
	Address m_addr;
};



template <class T>
AsyncTcpConnection<T>::AsyncTcpConnection(socket_ptr sock)
	: m_Socket(sock)
	, m_tPing(sock->get_io_service(), boost::posix_time::milliseconds(KEEP_ALIVE_MS))
	, m_bFirstTimeRead(true)
	, m_bAsyncWriteCalling(false)
{
	if (m_Socket)
	{
		auto ep = m_Socket->remote_endpoint();
		m_addr.addr = inet_addr(ep.address().to_string().c_str());
		m_addr.port = ep.port();
	}
}

template <class T>
AsyncTcpConnection<T>::~AsyncTcpConnection()
{
	this->Release();
}

template <class T>
size_t AsyncTcpConnection<T>::GetDataQueueSize()
{
	return m_dataList.size();
}

template <class T>
Address AsyncTcpConnection<T>::GetAddrPort()
{
	return m_addr;
}

template <class T>
std::string AsyncTcpConnection<T>::GetAddrStr()
{
	struct in_addr stInAddr;
	stInAddr.S_un.S_addr = this->GetAddrPort().addr;
	std::string saddr;
	saddr.append(inet_ntoa(stInAddr));
	saddr.append(":");
	char buffer[64] = { 0 };
	_itoa(this->GetAddrPort().port, buffer, 10);
	saddr.append(buffer);
	return saddr;
}

template <class T>
void AsyncTcpConnection<T>::DoRead()
{
	if (m_bFirstTimeRead)
	{
		ShareData sData = DataBuffer::Make(READ_BUF_LEN);
		//防止回调时对象已被销毁，导致溢出
		m_Socket->async_read_some(
			buffer(sData->Data(), sData->Length()),
			boost::bind(
				&AsyncTcpConnection::HandleRead,
				this,
				shared_from_this(),
				sData, _1, _2));

		m_tPing.async_wait(boost::bind(&AsyncTcpConnection::KeepAliveCallBack, _1, shared_from_this()));

		m_bFirstTimeRead = false;
	}
}

template <class T>
void AsyncTcpConnection<T>::KeepAliveCallBack(
	const boost::system::error_code &err, boost::shared_ptr<T> connPtr)
{
	if (connPtr)
		connPtr->KeepAliveHandler(err);
}

template <class T>
void AsyncTcpConnection<T>::HandleRead(
	boost::shared_ptr<T> connPtr,
	ShareData data,
	const boost::system::error_code& err,
	std::size_t rbytes)
{
	if (err) goto ReadError;
	else
	{
		//buffer被写入多少数据
		data->SetUsed(rbytes);
		this->TryParse(data);

		ShareData sData = DataBuffer::Make(READ_BUF_LEN);
		//防止回调时对象已被销毁，导致溢出
		m_Socket->async_read_some(
			buffer(sData->Data(), sData->Length()),
			boost::bind(
				&AsyncTcpConnection::HandleRead,
				this,
				shared_from_this(),
				sData, _1, _2));

		return;
	}

ReadError: 
	connPtr->Release();
}

template <class T>
void AsyncTcpConnection<T>::HandleWrite(
	boost::shared_ptr<T> connPtr,
	ShareData data,
	const boost::system::error_code& err,
	std::size_t wbytes)
{
	if (err) goto WriteError;

	//已写数据量
	data->SetUsed(data->Used() + wbytes);
	if (data->Used() < data->Length())
	{
		m_Socket->async_write_some(
			buffer(data->Data() + data->Used(), data->Length() - data->Used()),
			boost::bind(&AsyncTcpConnection::HandleWrite,
				this,
				shared_from_this(), data, _1, _2));
		return;
	}
	else if (data->Used() == data->Length())
	{
		std::lock_guard<std::mutex> lock(m_callingMutex);
		if (m_dataList.size() > 0)
		{
			std::lock_guard<std::mutex> lock(m_dataListMutex);
			ShareData sData = m_dataList.front();
			m_dataList.pop_front();
			m_Socket->async_write_some(
				buffer(sData->Data(), sData->Length()),
				boost::bind(&AsyncTcpConnection::HandleWrite,
					this,
					shared_from_this(), sData, _1, _2));
			return;
		}
		m_bAsyncWriteCalling = false;
		return;
	}

WriteError:
	connPtr->Release();
}

template <class T>
void AsyncTcpConnection<T>::SendShareData(const ShareData& data)
{
	{
		std::lock_guard<std::mutex> lock(m_dataListMutex);
		m_dataList.push_back(data);
	}

	if (!m_bAsyncWriteCalling)
	{
		std::lock_guard<std::mutex> lock(m_callingMutex);
		if (!m_bAsyncWriteCalling)
		{
			std::lock_guard<std::mutex> lock(m_dataListMutex);
			ShareData sData = m_dataList.front();
			m_dataList.pop_front();
			m_Socket->async_write_some( 
				buffer(sData->Data(), sData->Length()),
				boost::bind(&AsyncTcpConnection::HandleWrite,
					this,
					shared_from_this(), sData, _1, _2));
			m_bAsyncWriteCalling = true;
		}
	}
}

template <class T>
void AsyncTcpConnection<T>::Release()
{
	if (m_Socket)
	{
		boost::system::error_code err;
		m_Socket->cancel(err); 
		m_Socket->close(err);
		//m_Socket = NULL; 不能置为NULL,HandleRead还在使用，后面它返回错误
	}
	boost::system::error_code err;
	m_tPing.cancel(err);
	
}
