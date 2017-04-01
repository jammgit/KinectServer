#pragma once

#include "IKSKinectDataService.h"
#include "KSKDServiceStateMachine.h"
#include <boost\shared_ptr.hpp>

class IKSSession;
typedef boost::shared_ptr<IKSSession> IKSSessionPtr;
class IKSService;
typedef boost::shared_ptr<IKSService> IKSServicePtr;

class KSKinectDataService
	: public IKSKinectDataService
{
public:
	KSKinectDataService(IKSServicePtr service, IKSSessionPtr session);
	~KSKinectDataService();

	void DoFrame(const ShareFrame& frame) override;

	void SendEnd(eSvrEndType type, const std::string& devname) override;
protected:
	void ProcessStartReq(const ShareFrame& frame);
	void ProcessEndReq(const ShareFrame& frame);

private:
	KSKDServiceStateMachinePtr m_StateMachine;
	IKSSessionPtr m_Session;
	IKSServicePtr m_Service;
};
