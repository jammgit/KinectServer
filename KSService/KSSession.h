#pragma once

#include "IKSSession.h"
#include "../KSUtils/ProtocProcess.h"
#include "../KSUtils/AsyncTcpConnection.h"

class IKSCloudService;
class IKSSkeletonService;

class KSSession
	: public IKSSession
	, public AsyncTcpConnection
{
public:
	KSSession(socket_ptr sock);
	~KSSession();

	void RegisterAllService() override;

	void SendShareFrame(ShareFrame frame) override;

protected:
	void TryParse(const ShareData& data) override;
	void DoFrame(const ShareFrame& frame) override;

private:
	ProtocProcess m_protocProc;

	IKSCloudService *m_pCloudService;
	IKSSkeletonService *m_pSkeletonService;
};
