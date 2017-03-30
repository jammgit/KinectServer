#pragma once

#include "../KSUtils/AsyncTcpServer.h"
#include <boost\shared_ptr.hpp>
#include <string>

class KSSession;
typedef boost::shared_ptr<KSSession> KSSessionPtr;

class KSKinectDataServer;
typedef boost::shared_ptr<KSKinectDataServer> KSKinectDataServerPtr;

class IKSService
	: public AsyncTcpServer<IKSService>
{
public:
	IKSService(unsigned short port) : AsyncTcpServer<IKSService>(port) {};
	virtual ~IKSService() {};

	// new virtual func
	virtual void ReleaseSession(const std::string& guid) = 0;
	virtual KSKinectDataServerPtr GetColorServerPtr() = 0;
	virtual KSKinectDataServerPtr GetDepthServerPtr() = 0;
	virtual KSKinectDataServerPtr GetSkeleServerPtr() = 0;

};
