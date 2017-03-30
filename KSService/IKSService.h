#pragma once

#include <boost\shared_ptr.hpp>
#include <string>

class KSSession;
typedef boost::shared_ptr<KSSession> KSSessionPtr;

class AsyncTcpConnection;
typedef boost::shared_ptr<AsyncTcpConnection> AsyncTcpConnectionPtr;
class KSKinectDataServer;
typedef boost::shared_ptr<KSKinectDataServer> KSKinectDataServerPtr;

class IKSService
{
public:
	virtual ~IKSService() {};

	virtual void ReleaseSession(const std::string& guid) = 0;

	virtual KSKinectDataServerPtr GetColorServerPtr() = 0;
	virtual KSKinectDataServerPtr GetDepthServerPtr() = 0;
	virtual KSKinectDataServerPtr GetSkeleServerPtr() = 0;
	//virtual unsigned short GetColorPort() = 0;
	//virtual unsigned short GetDepthPort() = 0;
	//virtual unsigned short GetSkeletonPort() = 0;
};
