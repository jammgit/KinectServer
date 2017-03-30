#include "KSKinectDataServer.h"
#include "KSKinectDataEncoder.h"
#include "KSKinectDataSender.h"
#include "DataChannelProto.pb.h"
#include "../KSUtils/PortsMacro.h"

std::map<unsigned short, KSKinectDataEncoder::eSrcType>
	KSKinectDataServer::m_Port2SrcTypeMap = 
	{ 
		{ PORT_COLORDATA , KSKinectDataEncoder::SRC_TYPE_COLOR},
		{ PORT_DEPTHDATA , KSKinectDataEncoder::SRC_TYPE_DEPTH},
		{ PORT_SKELETONDATA , KSKinectDataEncoder::SRC_TYPE_SKELETON}
	};


KSKinectDataServer::KSKinectDataServer(unsigned short port)
	: AsyncTcpServer(port)
{
}

KSKinectDataServer::~KSKinectDataServer()
{
}

void KSKinectDataServer::WorkingFunc()
{
	AsyncTcpServer::Start();
}

void KSKinectDataServer::Stop()
{
	AsyncTcpServer::Stop();

	Thread::Stop();
}

bool KSKinectDataServer::RegisterCmdSock(
	const StrGUID& guid,
	AsyncTcpConnectionPtr conn)
{
	std::lock_guard<std::mutex> lock(m_MapMutex);
	m_Guid2CmdSockMap[guid] = conn;
	return true;
}

bool KSKinectDataServer::GetCmdSock(
	const StrGUID& guid,
	AsyncTcpConnectionPtr& conn)
{
	std::lock_guard<std::mutex> lock(m_MapMutex);
	auto iter = m_Guid2CmdSockMap.find(guid);
	if (iter != m_Guid2CmdSockMap.end())
	{
		conn = iter->second;
		return true;
	}
	return false;
}

bool KSKinectDataServer::RegisterDataSock(
	const StrGUID& guid,
	const std::string& devname,
	AsyncTcpConnectionPtr conn)
{
	std::lock_guard<std::mutex> lock(m_MapMutex);
	m_Guid2DataSockMap[guid][devname] = conn;
	return true;
}

bool KSKinectDataServer::UnRegisterAllSock(
	const StrGUID& guid,
	const std::string& devname)
{
	std::lock_guard<std::mutex> lock(m_MapMutex);

	auto iter = m_Guid2CmdSockMap.find(guid);
	if (iter != m_Guid2CmdSockMap.end())
		m_Guid2CmdSockMap.erase(iter);

	auto iter_1 = m_Guid2DataSockMap.find(guid);
	if (iter_1 != m_Guid2DataSockMap.end())
	{
		auto iter_2 = iter_1->second.find(devname);
		if (iter_2 != iter_1->second.end())
		{
			iter_1->second.erase(iter_2);
			if (iter_1->second.empty())
			{
				m_Guid2DataSockMap.erase(iter_1);
			}
		}
	}
	return true;
}


//void KSKinectDataServer::ReleaseSender(AsyncTcpConnectionPtr sender)
//{
//	std::lock_guard<std::mutex> lock(m_SenderListMutex);
//	auto iter = m_SenderPtrList.begin();
//	while (iter != m_SenderPtrList.end())
//	{
//		if (*iter == sender)
//		{
//			m_SenderPtrList.erase(iter);
//			break;
//		}
//		++iter;
//	}
//}



void KSKinectDataServer::CreateConnection(socket_ptr sock)
{
	KSKinectDataSenderPtr sender = NULL;

	sender = boost::make_shared<KSKinectDataSender>(
		shared_from_this(), sock, m_Port2SrcTypeMap[this->GetPort()]);

	sender->DoRead();
}