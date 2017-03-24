#pragma once

#include "../KSUtils/AsyncTcpServer.h"
#include "../KSUtils/Thread.h"
#include "../KSUtils/Singleton.h"
#include "IKSService.h"
#include "KSSession.h"
#include <mutex>

class KSSession;
typedef boost::shared_ptr<KSSession> KSSessionPtr;
typedef std::list<KSSessionPtr> KSSessionPtrList;

class KSService;
typedef boost::shared_ptr<KSService> KSServicePtr;

class KSKinectDataServer;
typedef boost::shared_ptr<KSKinectDataServer> KSKinectDataServerPtr;

//class KSService;
//extern template class AsyncTcpServer<KSService>;

class KSService
	: public IKSService
	, public AsyncTcpServer
	, public Thread
	, public Singleton<KSService>
{
public:
	KSService();
	~KSService();

	unsigned short GetColorPort() override;
	unsigned short GetDepthPort() override;
	unsigned short GetSkeletonPort() override;

	void ReleaseSession(AsyncTcpConnectionPtr session) override;
	void Stop() override;

protected:
	void CreateConnection(socket_ptr sock) override;

protected:
	void WorkingFunc() override;
	void Release();

private:
	std::mutex m_listMutex;
	KSSessionPtrList m_sessionList;

	KSKinectDataServerPtr m_ColorServerPtr;
	KSKinectDataServerPtr m_DepthServerPtr;
	KSKinectDataServerPtr m_SkeletonServerPtr;
};
