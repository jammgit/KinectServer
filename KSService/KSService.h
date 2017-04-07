#pragma once



#include "../KSUtils/Singleton.h"
#include "IKSService.h"
#include "KSSession.h"
#include "IKSClient.h"
#include <mutex>
#include <string>
#include <map>

class KSSession;
typedef boost::shared_ptr<KSSession> KSSessionPtr;
typedef std::list<KSSessionPtr> KSSessionPtrList;

class KSService;
typedef boost::shared_ptr<KSService> KSServicePtr;

using StrGUID = std::string;
typedef std::map<StrGUID, KSSessionPtr> KSSessionMap;

class KSService
	: public IKSService
	, public Singleton<KSService>
{
public:
	KSService();
	~KSService();

	void RegisterClient(IKSClient *client);

	KSKinectDataServerPtr GetColorServerPtr() override;
	KSKinectDataServerPtr GetDepthServerPtr() override;
	KSKinectDataServerPtr GetSkeleServerPtr() override;

	void ReleaseSession(const std::string& guid) override;
	void Stop() override;

protected:
	void CreateConnection(socket_ptr sock) override;

protected:
	void WorkingFunc() override;


private:

	std::mutex m_MapMutex;
	KSSessionMap m_SessionMap;

	KSKinectDataServerPtr m_ColorServerPtr;
	KSKinectDataServerPtr m_DepthServerPtr;
	KSKinectDataServerPtr m_SkeletonServerPtr;
};
