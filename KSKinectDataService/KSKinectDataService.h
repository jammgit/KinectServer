#pragma once

#include "IKSKinectDataService.h"

class KSSession;
class IKSService;

class KSKinectDataService
	: public IKSKinectDataService
{
public:
	KSKinectDataService(IKSService* service, KSSession *session);
	~KSKinectDataService();

	void DoFrame(const ShareFrame& frame) override;

	void SendEnd() override;
protected:
	void ProcessStartReq(const ShareFrame& frame);
	void ProcessEndReq(const ShareFrame& frame);

private:

	KSSession *m_pSession;
	IKSService *m_pService;
};
