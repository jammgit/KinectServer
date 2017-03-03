#include "AsyncTcpConnection.h"
#include <boost\bind.hpp>

AsyncTcpConnection::AsyncTcpConnection(socket_ptr sock)
	: m_Socket(sock)
	, m_bFirstTimeRead(true)
	, m_bAsyncWriteCalling(false)
{
}

AsyncTcpConnection::~AsyncTcpConnection()
{
	m_Socket->close();
}

void AsyncTcpConnection::DoRead()
{
	if (m_bFirstTimeRead)
	{
		ShareData sData = DataBuffer::Make(READ_BUF_LEN);
		//防止回调时对象已被销毁，导致溢出
		m_Socket->async_read_some(
			buffer(sData->Data(), sData->Length()),
			boost::bind(
				&AsyncTcpConnection::HandleRead,
				shared_from_this(),
				sData, _1, _2));
		m_bFirstTimeRead = false;
	}
}

void AsyncTcpConnection::HandleRead(
	ShareData data,
	const boost::system::error_code& err,
	std::size_t rbytes)
{
	if (err) goto ReadError;
	else
	{
		data->SetUsed(rbytes);
		this->TryParse(data);

		ShareData sData = DataBuffer::Make(READ_BUF_LEN);
		//防止回调时对象已被销毁，导致溢出
		m_Socket->async_read_some(
			buffer(sData->Data(), sData->Length()),
			boost::bind(
				&AsyncTcpConnection::HandleRead,
				shared_from_this(),
				sData, _1, _2));

		return;
	}

ReadError: // sock error, how to close.
	m_Socket->close();
}

void AsyncTcpConnection::HandleWrite(
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
				shared_from_this(), data, _1, _2));
		return;
	}
	else if (data->Used() == data->Length())
	{
		if (m_dataList.size() > 0)
		{
			std::lock_guard<std::mutex> lock(m_dataListMutex);
			ShareData sData = m_dataList.front();
			m_dataList.pop_front();

			m_Socket->async_write_some(
				buffer(sData->Data(), sData->Length()),
				boost::bind(&AsyncTcpConnection::HandleWrite,
					shared_from_this(), sData, _1, _2));
			return;
		}
	}

	m_bAsyncWriteCalling = false;
	return;

WriteError:
	m_Socket->close();
}

void AsyncTcpConnection::SendShareData(const ShareData& data)
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
					shared_from_this(), sData, _1, _2));
			m_bAsyncWriteCalling = true;
		}
	}
}