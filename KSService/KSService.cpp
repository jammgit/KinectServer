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
	static bool first = true;
	if (first)
	{
		KinectDataCapturer::GetInstance()->RegisterDevStatusCallBack();
		first = false;
	}
	if (!m_ColorServerPtr) { m_ColorServerPtr = boost::make_shared<KSKinectDataServer>(PORT_COLORDATA); m_ColorServerPtr->Thread::Start(); }
	if (!m_DepthServerPtr) { m_DepthServerPtr = boost::make_shared<KSKinectDataServer>(PORT_DEPTHDATA); m_DepthServerPtr->Thread::Start(); }
	if (!m_SkeletonServerPtr) { m_SkeletonServerPtr = boost::make_shared<KSKinectDataServer>(PORT_SKELETONDATA); m_SkeletonServerPtr->Thread::Start(); }
	AsyncTcpServer::Start();
}

void KSService::Stop()
{
	// 结束服务线程
	IKSService::Stop();

	// 删除服务器的信息
	std::lock_guard<std::mutex> lock(m_MapMutex);
	auto iter = m_SessionMap.begin();
	while (iter != m_SessionMap.end())
	{
		m_ColorServerPtr->UnregisterCmdSock(iter->first);
		m_DepthServerPtr->UnregisterCmdSock(iter->first);
		m_SkeletonServerPtr->UnregisterCmdSock(iter->first);

		iter->second->Close();
		m_SessionMap.erase(iter++);
	}

	// 结束数据服务器
	if (m_ColorServerPtr) { m_ColorServerPtr->Stop(); m_ColorServerPtr = NULL; }
	if (m_DepthServerPtr) { m_DepthServerPtr->Stop(); m_DepthServerPtr = NULL; }
	if (m_SkeletonServerPtr) { m_SkeletonServerPtr->Stop(); m_SkeletonServerPtr = NULL; }
	
	// 删除回话信息
	//std::lock_guard<std::mutex> lock_1(m_MapMutex);
	//m_SessionMap.clear();
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

		return;
	}
	else
	{
		
	}
}

void KSService::ReleaseSession(const std::string& guid)
{
	std::lock_guard<std::mutex> lock(m_MapMutex);
	auto iter = m_SessionMap.find(guid);
	if (iter != m_SessionMap.end())
	{
		m_SessionMap.erase(iter);

		m_ColorServerPtr->UnregisterCmdSock(guid);
		m_DepthServerPtr->UnregisterCmdSock(guid);
		m_SkeletonServerPtr->UnregisterCmdSock(guid);

	}

}

void KSService::RegisterClient(IKSClient *client)
{
	KinectDataCapturer::GetInstance()->RegisterClient(client);
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

