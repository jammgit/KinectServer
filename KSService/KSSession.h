#pragma once

#include "IKSSession.h"
#include "../KSUtils/ProtocProcess.h"

#include <string>

class IKSKinectDataService;
typedef boost::shared_ptr<IKSKinectDataService> IKSKinectDataServicePtr;
class IKSService;
typedef boost::shared_ptr<IKSService> IKSServicePtr;

class KSSession
	: public IKSSession
{
public:
	KSSession(IKSServicePtr service, socket_ptr sock, std::string guid);
	~KSSession();

	void RegisterAllService() override;
	IKSKinectDataServicePtr KinectDataService() override;
	std::string StrGuid() override;
	void DoFrame(const ShareFrame& frame) override;
	void Close() override;

	void SendShareFrame(ShareFrame frame) override;
	
protected:
	void TryParse(const ShareData& data) override;
	void KeepAliveHandler(const boost::system::error_code &) override;
	void Release() override;

protected:
	inline void ReqConnect(const ShareFrame& frame);
	inline void ReqUnConnect(const ShareFrame& frame);
	inline void ReqDeviceList(const ShareFrame& frame);
	inline void DoPing(ShareFrame frame);

private:
	std::string m_PeerHostname;
	std::string m_StrGuid;
	bool m_bRecvPing;
	ProtocProcess m_protocProc;

	IKSKinectDataServicePtr m_KDService;
	IKSServicePtr m_Service;
};
