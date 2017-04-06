#pragma once

#include "../KSUtils/AsyncTcpServer.h"
#include "../KSUtils/Thread.h"
#include <boost\shared_ptr.hpp>
#include <string>

class KSSession;
typedef boost::shared_ptr<KSSession> KSSessionPtr;

class KSKinectDataServer;
typedef boost::shared_ptr<KSKinectDataServer> KSKinectDataServerPtr;

class IKSService;
typedef boost::shared_ptr<IKSService> IKSServicePtr;

class IKSClient;

class IKSService
	: public AsyncTcpServer<IKSService>
	, public Thread
{
public:
	IKSService(unsigned short port) : AsyncTcpServer<IKSService>(port) {};
	virtual ~IKSService() {};

	void Start() override {
		Thread::Start();
	}
	void Stop() override {
		AsyncTcpServer::Stop();
		Thread::Stop();
	}

	// new virtual func
	virtual void ReleaseSession(const std::string& guid) = 0;
	virtual KSKinectDataServerPtr GetColorServerPtr() = 0;
	virtual KSKinectDataServerPtr GetDepthServerPtr() = 0;
	virtual KSKinectDataServerPtr GetSkeleServerPtr() = 0;
	virtual void RegisterClient(IKSClient *client) = 0;
};
