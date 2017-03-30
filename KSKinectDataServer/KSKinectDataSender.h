#pragma once

#include "../KSUtils/Thread.h"
#include "../KSUtils/AsyncTcpConnection.h"
#include "../KSUtils/ProtocProcess.h"
#include "../IKSKinectDataCapture/EncodedColorFrame.h"
#include "../IKSKinectDataCapture/EncodedDepthFrame.h"
#include "../IKSKinectDataCapture/SkeletonFrame.h"
#include "KSKinectDataEncoder.h"
#include <mutex>

using StrGUID = std::string;

class KSKinectDataEncoder;
typedef boost::shared_ptr<KSKinectDataEncoder> KSKinectDataEncoderPtr;

class KSKinectDataServer;
typedef boost::shared_ptr<KSKinectDataServer> KSKinectDataServerPtr;

//class AsyncTcpServer;
//typedef boost::shared_ptr<AsyncTcpServer> AsyncTcpServerPtr;

class KSKinectDataSender;
typedef boost::shared_ptr<KSKinectDataSender> KSKinectDataSenderPtr;

class KSKinectDataSender
	: public AsyncTcpConnection<KSKinectDataSender>
{
	typedef enum
	{
		CMD_TYPE_DATA_CHANNEL = 3,
	}eCmdType;
	typedef enum
	{
		CMD_NUM_DATA_CHANNEL = 1,
	}eCmdNum;
public:
	KSKinectDataSender(
		KSKinectDataServerPtr server,
		socket_ptr sock,
		KSKinectDataEncoder::eSrcType type);
	~KSKinectDataSender();

	void Send264Frame(Middle264FramePtr frame);
	void SendSkeletonFrame(SkeletonFramePtr frame);
	void DeviceUnlink();

	void Close();

//protected:
	void TryParse(const ShareData& data) override;
	void SendShareFrame(ShareFrame frame) {};
	void Release() override;

	inline void EndProcess();

private:
	KSKinectDataServerPtr m_pServer;

	ProtocProcess m_ProtoProcess;

	StrGUID m_StrGuid;
	std::string m_DevName;
	KSKinectDataEncoder::eSrcType m_type;

	std::mutex m_EncoderMutex;
	KSKinectDataEncoderPtr m_EncoderPtr;
	
	std::mutex m_ReleaseMutex;
};
