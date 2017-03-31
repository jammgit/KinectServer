#include "KSService.h"
#include "../IKSKinectDataCapture/KinectDataCapturer.h"
#include "../KSLogService/KSLogService.h"
#include "../KSUtils/PortsMacro.h"
#include "../KSUtils/GuidProcessUtil.h"
#include "../KSKinectDataServer/KSKinectDataServer.h"

KSService::KSService()
	: IKSService(PORT_KSSERVICE)
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

	std::lock_guard<std::mutex> lock(m_MapMutex);
	m_SessionMap.clear();
}

void KSService::CreateConnection(socket_ptr sock)
{
	std::string guid = GuidProcessUtil::CreateStrGuid();
	KSSessionPtr session = boost::make_shared<KSSession>(
		shared_from_this(), sock, guid);
	ShareFrame frame;
	if (m_SessionMap.size() < 65535)
	{
		session->RegisterAllService();
		session->DoRead();
		
		std::lock_guard<std::mutex> lock(m_MapMutex);
		m_SessionMap[guid] = session;

		m_ColorServerPtr->RegisterCmdSock(guid, session);
		m_DepthServerPtr->RegisterCmdSock(guid, session);
		m_SkeletonServerPtr->RegisterCmdSock(guid, session);

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

void KSService::ReleaseSession(const std::string& guid)
{
	std::lock_guard<std::mutex> lock(m_MapMutex);
	auto iter = m_SessionMap.find(guid);
	if (iter != m_SessionMap.end())
	{

		KSLogService::GetInstance()->OutputClient(
			iter->second->GetAddrStr().c_str(), false);

		m_SessionMap.erase(iter);

		m_ColorServerPtr->UnregisterCmdSock(guid);
		m_DepthServerPtr->UnregisterCmdSock(guid);
		m_SkeletonServerPtr->UnregisterCmdSock(guid);

	}

}


KSKinectDataServerPtr KSService::GetColorServerPtr()
{
	return m_ColorServerPtr;
}

KSKinectDataServerPtr KSService::GetDepthServerPtr()
{
	return m_DepthServerPtr;
}

KSKinectDataServerPtr KSService::GetSkeleServerPtr()
{
	return m_SkeletonServerPtr;
}

