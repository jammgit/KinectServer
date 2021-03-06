#pragma once

#include "../KSUtils/AsyncTcpServer.h"
#include "../KSUtils/AsyncTcpConnection.h"
#include "../KSUtils/Thread.h"
#include "KSKinectDataSender.h"

#include <mutex>
#include <list>
#include <map>

using StrGuid = std::string;

class IKSSession;
typedef boost::shared_ptr<IKSSession> IKSSessionPtr;

typedef std::list<KSKinectDataSenderPtr> KSKinectDataSenderPtrList;

typedef std::map<std::string, KSKinectDataSenderPtr> DevName2KinectDataSenderMap;


class KSKinectDataServer
	: public AsyncTcpServer<KSKinectDataServer>
	, public Thread
{
	friend class AsyncTcpServer<KSKinectDataServer>;

public:
	KSKinectDataServer(unsigned short port);
	~KSKinectDataServer();

	void Stop() override;

	//cmd socket接收到开始请求后注册信息
	bool RegisterCmdSock(
		const StrGUID& guid,
		IKSSessionPtr conn);

	bool UnregisterCmdSock(
		const StrGUID& guid);

	bool GetCmdSock(
		const StrGUID& guid,
		IKSSessionPtr& conn);

	//data socket 接收到数据后注册信息
	bool RegisterDataSock(
		const StrGUID& guid,
		const std::string& devname,
		KSKinectDataSenderPtr conn);

	// 结束数据传输后执行删除信息
	bool UnRegisterDataSock(
		const StrGUID& guid,
		const std::string& devname);

protected:
	void WorkingFunc() override;
	void CreateConnection(socket_ptr sock) override;

private:
	
	std::mutex m_CmdSockMutex;
	std::map<StrGuid, IKSSessionPtr> m_Guid2CmdSockMap; //数据信道获取控制信道socket
	std::mutex m_DataSockMutex;
	std::map<StrGuid, DevName2KinectDataSenderMap> m_Guid2DataSockMap; //控制信道socket拥有的数据socket
	
	static std::map<unsigned short, KSKinectDataEncoder::eSrcType> m_Port2SrcTypeMap;
};
