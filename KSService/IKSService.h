#pragma once

#include <boost\shared_ptr.hpp>

class KSSession;
typedef boost::shared_ptr<KSSession> KSSessionPtr;

class AsyncTcpConnection;
typedef boost::shared_ptr<AsyncTcpConnection> AsyncTcpConnectionPtr;

class IKSService
{
public:
	virtual ~IKSService() {};

	virtual void ReleaseSession(AsyncTcpConnectionPtr session) = 0;

	virtual unsigned short GetColorPort() = 0;
	virtual unsigned short GetDepthPort() = 0;
	virtual unsigned short GetSkeletonPort() = 0;
};
