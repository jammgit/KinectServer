#pragma once

#include "../KSUtils/AsyncTcpServer.h"
#include "../KSUtils/AsyncTcpConnection.h"
#include "../KSUtils/Thread.h"
#include "KSKinectDataSender.h"

#include <mutex>
#include <list>
#include <map>

typedef std::list<KSKinectDataSenderPtr> KSKinectDataSenderPtrList;

//class KSKinectDataServer;
//extern template class AsyncTcpServer<KSKinectDataServer>;

class AsyncTcpConnection;
typedef boost::shared_ptr<AsyncTcpConnection> AsyncTcpConnectionPtr;

class KSKinectDataServer
	: public AsyncTcpServer
	, public Thread
{
public:
	KSKinectDataServer(unsigned short port);
	~KSKinectDataServer();

	static void SetDataAddrByCmdAddr(
		const Address& cmdaddr,
		const Addresses& dataaddrs);

	static bool ResetDataAddrByCmdAddr(
		const Address& cmdaddr,
		Addresses& dataaddrs);

	static void SetSockByCmdAddr(
		const Address& cmdaddr,
		const AsyncTcpConnectionPtr& conn);
	static void ResetSockByCmdAddr(
		const Address& cmdaddr);

	static void CloseSenders(const Addresses& addrs);

	void ReleaseSender(AsyncTcpConnectionPtr sender);

	bool IsColor();
	bool IsDepth();
	bool IsSkeleton();

	void Stop() override;

protected:
	void WorkingFunc() override;
	void CreateConnection(socket_ptr sock) override;

private:
	static std::mutex m_SenderListMutex;
	static KSKinectDataSenderPtrList m_SenderPtrList;//数据信道套接字

	static std::mutex m_ClientSockMutex;
	static std::map<Address, Addresses> m_ClientAddrMap; //<控制信道地址，数据信道地址>

	static std::mutex m_CmdSockMutex;
	static std::map<Address, AsyncTcpConnectionPtr> m_CmdSockMap; //<控制信道地址，控制信道套接字>
};
