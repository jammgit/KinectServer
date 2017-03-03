#pragma once

#include "IKSService.h"
#include "../KSUtils/AsyncTcpServer.h"
#include "../KSUtils/Thread.h"
#include "../KSUtils/Singleton.h"
#include "KSSession.h"

class KSSession;
typedef boost::shared_ptr<KSSession> KSSessionPtr;
typedef std::list<KSSessionPtr> KSSessionPtrList;

class KSService
	: public IKSService
	, public AsyncTcpServer
	, public Thread
	, public Singleton<KSService>
{
public:
	KSService();
	~KSService();

	void Stop() override;
protected:
	void CreateConnection(socket_ptr sock) override;

protected:
	void WorkingFunc() override;

private:
	KSSessionPtrList m_sessionList;
};
