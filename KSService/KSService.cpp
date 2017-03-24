#include "KSService.h"
#include "../IKSKinectDataCapture/KinectDataCapturer.h"
#include "../KSLogService/KSLogService.h"
#include "../KSUtils/PortsMacro.h"
#include "../KSKinectDataService/KSKinectDataServer.h"

KSService::KSService()
	: AsyncTcpServer(PORT_KSSERVICE)
	, m_ColorServerPtr(NULL)
	, m_DepthServerPtr(NULL)
	, m_SkeletonServerPtr(NULL)
{
}

KSService::~KSService()
{
	this->Release();
}

void KSService::WorkingFunc()
{
	KinectDataCapturer::GetInstance()->RegisterDevStatusCallBack();
	if (!m_ColorServerPtr) { m_ColorServerPtr = boost::make_shared<KSKinectDataServer>(PORT_COLORDATA); m_ColorServerPtr->Thread::Start(); }
	if (!m_DepthServerPtr) { m_DepthServerPtr = boost::make_shared<KSKinectDataServer>(PORT_DEPTHDATA); m_DepthServerPtr->Thread::Start(); }
	if (!m_SkeletonServerPtr) { m_SkeletonServerPtr = boost::make_shared<KSKinectDataServer>(PORT_SKELETONDATA); m_SkeletonServerPtr->Thread::Start(); }
	AsyncTcpServer::Start();
}

void KSService::Stop()
{
	AsyncTcpServer::Stop();

	Thread::Stop();

	std::lock_guard<std::mutex> lock(m_listMutex);
	m_sessionList.clear();
}

void KSService::CreateConnection(socket_ptr sock)
{
	KSSessionPtr session = boost::make_shared<KSSession>(this, sock);
	ShareFrame frame;
	if (m_sessionList.size() < 65535)
	{
		session->RegisterAllService();
		session->DoRead();
		std::lock_guard<std::mutex> lock(m_listMutex);
		m_sessionList.push_back(session);

		KSLogService::GetInstance()->OutputMessage("Create Connection\n");

		KSLogService::GetInstance()->OutputClient(
			session->GetAddrStr().c_str(), true);
		return;
	}
	else
	{
		KSLogService::GetInstance()->OutputMessage("Create Connection fail\n");
	}
}

void KSService::ReleaseSession(AsyncTcpConnectionPtr session)
{
	std::lock_guard<std::mutex> lock(m_listMutex);
	auto iter = m_sessionList.begin();
	for (; iter != m_sessionList.end(); )
	{
		if (*iter == session)
		{
			m_sessionList.erase(iter);
			break;
		}
		++iter;
	}
	KSLogService::GetInstance()->OutputClient(
		session->GetAddrStr().c_str(), false);
}

unsigned short KSService::GetColorPort()
{
	if (m_ColorServerPtr)
		return m_ColorServerPtr->GetPort();
	return 0;
}
unsigned short KSService::GetDepthPort()
{
	if (m_DepthServerPtr)
		return m_DepthServerPtr->GetPort();
	return 0;
}
unsigned short KSService::GetSkeletonPort()
{
	if (m_SkeletonServerPtr)
		return m_SkeletonServerPtr->GetPort();
	return 0;
}

void KSService::Release()
{
}