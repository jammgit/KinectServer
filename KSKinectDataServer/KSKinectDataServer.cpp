#include "KSKinectDataServer.h"
#include "KSKinectDataEncoder.h"
#include "KSKinectDataSender.h"
#include "../KSUtils/PortsMacro.h"

std::mutex KSKinectDataServer::m_SenderListMutex;
std::mutex KSKinectDataServer::m_ClientSockMutex;
std::mutex KSKinectDataServer::m_CmdSockMutex;

KSKinectDataSenderPtrList KSKinectDataServer::m_SenderPtrList;//数据信道套接字

std::map<Address, Addresses> KSKinectDataServer::m_ClientAddrMap; //<控制信道地址，数据信道地址>

std::map<Address, AsyncTcpConnectionPtr> KSKinectDataServer::m_CmdSockMap; //<控制信道地址，控制信道套接字>

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

void KSKinectDataServer::SetDataAddrByCmdAddr(
	const Address& cmdaddr,
	const Addresses& dataaddrs)
{
	std::lock_guard<std::mutex> lock(m_ClientSockMutex);
	m_ClientAddrMap[cmdaddr] = dataaddrs;
}

bool KSKinectDataServer::ResetDataAddrByCmdAddr(
	const Address& cmdaddr,
	Addresses& dataaddrs)
{
	std::lock_guard<std::mutex> lock(m_ClientSockMutex);
	auto iter = m_ClientAddrMap.find(cmdaddr);
	if (iter == m_ClientAddrMap.end())
		return false;
	dataaddrs = iter->second;
	m_ClientAddrMap.erase(iter);
	return true;
}

void KSKinectDataServer::SetSockByCmdAddr(
	const Address& cmdaddr,
	const AsyncTcpConnectionPtr& conn)
{
	std::lock_guard<std::mutex> lock(m_CmdSockMutex);
	m_CmdSockMap[cmdaddr] = conn;
}

void KSKinectDataServer::ResetSockByCmdAddr(
	const Address& cmdaddr)
{
	std::lock_guard<std::mutex> lock(m_CmdSockMutex);
	auto iter = m_CmdSockMap.find(cmdaddr);
	if (iter == m_CmdSockMap.end())
		return;
	m_CmdSockMap.erase(iter);
}

void KSKinectDataServer::CloseSenders(const Addresses& addrs)
{
	std::lock_guard<std::mutex> lock(m_SenderListMutex);
	for (int i = 0; i < 3; ++i)
	{
		auto iter = m_SenderPtrList.begin();
		for (; iter != m_SenderPtrList.end(); ++iter)
		{
			if ((*iter)->GetAddrPort() == addrs.addr[i])
			{
				(*iter)->Close();
				m_SenderPtrList.erase(iter);
				break;
			}
		}
	}
}

void KSKinectDataServer::ReleaseSender(AsyncTcpConnectionPtr sender)
{
	std::lock_guard<std::mutex> lock(m_SenderListMutex);
	auto iter = m_SenderPtrList.begin();
	while (iter != m_SenderPtrList.end())
	{
		if (*iter == sender)
		{
			m_SenderPtrList.erase(iter);
			break;
		}
		++iter;
	}
}

bool KSKinectDataServer::IsColor()
{
	return GetPort() == PORT_COLORDATA;
}

bool KSKinectDataServer::IsDepth()
{
	return GetPort() == PORT_DEPTHDATA;
}

bool KSKinectDataServer::IsSkeleton()
{
	return GetPort() == PORT_SKELETONDATA;
}

void KSKinectDataServer::Stop()
{
	AsyncTcpServer::Stop();

	Thread::Stop();
}

void KSKinectDataServer::CreateConnection(socket_ptr sock)
{
	std::lock_guard<std::mutex> lock(m_SenderListMutex);
	KSKinectDataSenderPtr sender = NULL;

	switch (GetPort())
	{
	case PORT_COLORDATA:
		sender = boost::make_shared<KSKinectDataSender>(
			this, sock, KSKinectDataEncoder::SRC_TYPE_COLOR);
		break;
	case PORT_DEPTHDATA:
		sender = boost::make_shared<KSKinectDataSender>(
			this, sock, KSKinectDataEncoder::SRC_TYPE_DEPTH);
		break;
	case PORT_SKELETONDATA:
		sender = boost::make_shared<KSKinectDataSender>(
			this, sock, KSKinectDataEncoder::SRC_TYPE_SKELETON);
		break;
	default:
		break;
	}

	sender->DoRead();
	sender->Open();

	m_SenderPtrList.push_back(sender);
}