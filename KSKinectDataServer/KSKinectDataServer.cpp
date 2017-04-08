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
	: AsyncTcpServer<KSKinectDataServer>(port)
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
	IKSSessionPtr conn)
{
	std::lock_guard<std::mutex> lock(m_CmdSockMutex);
	m_Guid2CmdSockMap[guid] = conn;
	return true;
}

bool KSKinectDataServer::UnregisterCmdSock(
	const StrGUID& guid)
{
	{
		std::lock_guard<std::mutex> lock(m_CmdSockMutex);
		auto iter = m_Guid2CmdSockMap.find(guid);
		if (iter != m_Guid2CmdSockMap.end())
		{
			m_Guid2CmdSockMap.erase(iter);
		}
	}
	{
		std::lock_guard<std::mutex> lock(m_DataSockMutex);
		auto iter = m_Guid2DataSockMap.find(guid);
		if (iter != m_Guid2DataSockMap.end())
		{
			auto iter_2 = iter->second.begin();
			while (iter_2 != iter->second.end())
			{
				iter_2->second->Close();
				++iter_2;
			}
			iter->second.clear();
			m_Guid2DataSockMap.erase(iter);
		}
	}
	return true;
}

bool KSKinectDataServer::GetCmdSock(
	const StrGUID& guid,
	IKSSessionPtr& conn)
{
	std::lock_guard<std::mutex> lock(m_CmdSockMutex);
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
	KSKinectDataSenderPtr conn)
{
	std::lock_guard<std::mutex> lock(m_DataSockMutex);
	m_Guid2DataSockMap[guid][devname] = conn;
	return true;
}

bool KSKinectDataServer::UnRegisterDataSock(
	const StrGUID& guid,
	const std::string& devname)
{
	std::lock_guard<std::mutex> lock(m_DataSockMutex);
	auto iter_1 = m_Guid2DataSockMap.find(guid);
	if (iter_1 != m_Guid2DataSockMap.end())
	{
		auto iter_2 = iter_1->second.find(devname);
		if (iter_2 != iter_1->second.end())
		{
			iter_2->second->Close();
			iter_1->second.erase(iter_2);
		}
	}
	return true;
}

void KSKinectDataServer::CreateConnection(socket_ptr sock)
{
	KSKinectDataSenderPtr sender = NULL;

	sender = boost::make_shared<KSKinectDataSender>(
		shared_from_this(), sock, m_Port2SrcTypeMap[this->GetPort()]);

	sender->DoRead();
}