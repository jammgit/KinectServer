#pragma once

#include "../KSUtils/AsyncTcpServer.h"
#include "../KSUtils/AsyncTcpConnection.h"
#include "../KSUtils/Thread.h"
#include "KSKinectDataSender.h"

#include <mutex>
#include <list>
#include <map>

using StrGuid = std::string;

class AsyncTcpConnection;
typedef boost::shared_ptr<AsyncTcpConnection> AsyncTcpConnectionPtr;

typedef std::list<KSKinectDataSenderPtr> KSKinectDataSenderPtrList;

typedef std::map<std::string, AsyncTcpConnectionPtr> DevName2KinectDataSenderMap;


class KSKinectDataServer
	: public AsyncTcpServer
	, public Thread
{
public:
	KSKinectDataServer(unsigned short port);
	~KSKinectDataServer();

	void Stop() override;

	//cmd socket���յ���ʼ�����ע����Ϣ
	bool RegisterCmdSock(
		const StrGUID& guid,
		AsyncTcpConnectionPtr conn);
	bool GetCmdSock(
		const StrGUID& guid,
		AsyncTcpConnectionPtr& conn);

	//data socket ���յ����ݺ�ע����Ϣ
	bool RegisterDataSock(
		const StrGUID& guid,
		const std::string& devname,
		AsyncTcpConnectionPtr conn); 

	//���ﻹ�����⣡������
	// �������ݴ����ִ��ɾ����Ϣ
	bool UnRegisterAllSock(
		const StrGUID& guid,
		const std::string& devname);

protected:
	void WorkingFunc() override;
	void CreateConnection(socket_ptr sock) override;

private:
	
	std::mutex m_MapMutex;
	std::map<StrGuid, AsyncTcpConnectionPtr> m_Guid2CmdSockMap; //�����ŵ���ȡ�����ŵ�socket
	std::map<StrGuid, DevName2KinectDataSenderMap> m_Guid2DataSockMap; //�����ŵ�socketӵ�е�����socket
	
	static std::map<unsigned short, KSKinectDataEncoder::eSrcType> m_Port2SrcTypeMap;
};
