#pragma once

#include "../KSUtils/Thread.h"
#include "../KSUtils/AsyncTcpConnection.h"
#include "../IKSKinectDataCapture/EncodedColorFrame.h"
#include "../IKSKinectDataCapture/EncodedDepthFrame.h"
#include "../IKSKinectDataCapture/SkeletonFrame.h"
#include "KSKinectDataEncoder.h"
#include <mutex>

class KSKinectDataEncoder;
typedef boost::shared_ptr<KSKinectDataEncoder> KSKinectDataEncoderPtr;

class KSKinectDataServer;
typedef boost::shared_ptr<KSKinectDataServer> KSKinectDataServerPtr;

class KSKinectDataSender;
typedef boost::shared_ptr<KSKinectDataSender> KSKinectDataSenderPtr;

//class KSKinectDataSender;
//extern template class AsyncTcpConnection<KSKinectDataSender>;

class KSKinectDataSender
	: public AsyncTcpConnection
{
public:
	KSKinectDataSender(
		KSKinectDataServer *server,
		socket_ptr sock,
		KSKinectDataEncoder::eSrcType type);
	~KSKinectDataSender();

	void Open() override;
	void Close() override;

	void Send264Frame(Middle264FramePtr frame) override;
	void SendSkeletonFrame(SkeletonFramePtr frame) override;

protected:
	void TryParse(const ShareData& data) override;
	void SendShareFrame(ShareFrame frame) {};
	void Release() override;

private:
	std::mutex m_EncoderMutex;
	KSKinectDataEncoderPtr m_EncoderPtr;
	std::string m_deviceName;
	KSKinectDataEncoder::eSrcType m_type;

	KSKinectDataServer *m_pServer;
};
