#pragma once

#include "IKSSession.h"
#include "../KSUtils/ProtocProcess.h"
#include "../KSUtils/AsyncTcpConnection.h"

class IKSKinectDataService;
class IKSService;

class KSSession
	: public IKSSession
	, public AsyncTcpConnection
{
public:
	KSSession(IKSService* service, socket_ptr sock);
	~KSSession();

	void RegisterAllService() override;

	void SendShareFrame(ShareFrame frame) override;

protected:
	void TryParse(const ShareData& data) override;
	void DoFrame(const ShareFrame& frame) override;
	void Release() override;

private:
	ProtocProcess m_protocProc;

	IKSKinectDataService *m_pKDService;
	IKSService *m_pService;
};
