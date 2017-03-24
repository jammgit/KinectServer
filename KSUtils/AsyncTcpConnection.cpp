#include "AsyncTcpConnection.h"

AsyncTcpConnection::AsyncTcpConnection(socket_ptr sock)
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

AsyncTcpConnection::~AsyncTcpConnection()
{
	this->Release();
}

size_t AsyncTcpConnection::GetDataQueueSize()
{
	return m_dataList.size();
}

Address AsyncTcpConnection::GetAddrPort()
{
	return m_addr;
}

std::string AsyncTcpConnection::GetAddrStr()
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
				this,
				shared_from_this(),
				sData, _1, _2));
		m_bFirstTimeRead = false;

		//printf("In Do read\n");
	}
	//printf("Do read finish\n");
}

void AsyncTcpConnection::HandleRead(
	AsyncTcpConnectionPtr connPtr,
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

void AsyncTcpConnection::HandleWrite(
	AsyncTcpConnectionPtr connPtr,
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

void AsyncTcpConnection::Release()
{
	if (m_Socket)
	{
		m_Socket->close();
		m_Socket = NULL;
	}
}