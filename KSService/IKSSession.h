#pragma once

#include "../KSUtils/ShareFrame.h"
#include "../KSUtils/AsyncTcpConnection.h"

class IKSKinectDataService;
typedef boost::shared_ptr<IKSKinectDataService> IKSKinectDataServicePtr;

class IKSSession
	: public AsyncTcpConnection<IKSSession>
{
	friend class AsyncTcpConnection<IKSSession>;

public:
	enum
	{
		CMD_TYPE_CONNECT = 0x01,
		CMD_TYPE_KINECT = 0x02,
		CMD_TYPE_PING = 0x63,
	};

	enum
	{
		SUCCESS = 1,
		FAIL = -100,
	};

	enum
	{
		CMD_NUM_CLI_CONN_REQ = 1,
		CMD_NUM_CLI_UNCONN_REQ = 2,
		CMD_NUM_CLI_DEVICES_REQ = 3,
		CMD_NUM_SVR_CONN_RESP = 100,
		CMD_NUM_SVR_DEVICES_RESP = 101,

		CMD_NUM_CLI_PING = 1,
		CMD_NUM_SVR_PING = 100,
	};

public:
	IKSSession(socket_ptr sock)
		: AsyncTcpConnection<IKSSession>(sock) {};
	virtual ~IKSSession() {};

	virtual void RegisterAllService() = 0;
	virtual IKSKinectDataServicePtr KinectDataService() = 0;
	virtual std::string StrGuid() = 0;
	virtual void DoFrame(const ShareFrame& frame) = 0;

	virtual void SendShareFrame(ShareFrame frame) = 0;

protected:
	void TryParse(const ShareData& data) override {};
	void KeepAliveHandler(const boost::system::error_code &) override {};
	void Release() override { AsyncTcpConnection::Release(); };
};
