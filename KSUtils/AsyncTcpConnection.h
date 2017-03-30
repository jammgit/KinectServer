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

public: //模板不支持protected属性的继承，应是protected成员
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



template <class T>
AsyncTcpConnection<T>::AsyncTcpConnection(socket_ptr sock)
	: m_Socket(sock)
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
		m_bFirstTimeRead = false;

		//printf("In Do read\n");
	}
	//printf("Do read finish\n");
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
		//printf("接收到[%d]bytes数据\n", rbytes);
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

ReadError: // sock error, how to close.
		   //printf("socket error\n");
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
		printf("2async_write_some begin\n");
		m_Socket->async_write_some(
			buffer(data->Data() + data->Used(), data->Length() - data->Used()),
			boost::bind(&AsyncTcpConnection::HandleWrite,
				this,
				shared_from_this(), data, _1, _2));
		printf("2async_write_some end\n");
		return;
	}
	else if (data->Used() == data->Length())
	{
		//printf("write success\n");
		if (m_dataList.size() > 0)
		{
			std::lock_guard<std::mutex> lock(m_dataListMutex);
			ShareData sData = m_dataList.front();
			m_dataList.pop_front();
			printf("1async_write_some begin\n");
			m_Socket->async_write_some(
				buffer(sData->Data(), sData->Length()),
				boost::bind(&AsyncTcpConnection::HandleWrite,
					this,
					shared_from_this(), sData, _1, _2));
			printf("1async_write_some end\n");
			return;
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_callingMutex);
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
			printf("0async_write_some begin\n");
			m_Socket->async_write_some(
				buffer(sData->Data(), sData->Length()),
				boost::bind(&AsyncTcpConnection::HandleWrite,
					this,
					shared_from_this(), sData, _1, _2));
			printf("0async_write_some end\n");
			m_bAsyncWriteCalling = true;
		}
	}
}

template <class T>
void AsyncTcpConnection<T>::Release()
{
	if (m_Socket)
	{
		m_Socket->close();
		m_Socket = NULL;
	}
}
