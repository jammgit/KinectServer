#pragma once

#include "../KSUtils/Thread.h"
#include "../IKSKinectDataCapture/KinectDataCaptureQueue.h"
#include <string>
#include <boost\shared_ptr.hpp>
#include <mutex>

//class AsyncTcpConnection;
//typedef boost::shared_ptr<AsyncTcpConnection> AsyncTcpConnectionPtr;

class KSKinectDataSender;
typedef boost::shared_ptr<KSKinectDataSender> KSKinectDataSenderPtr;

class KSKinectDataEncoder;
typedef boost::shared_ptr<KSKinectDataEncoder> KSKinectDataEncoderPtr;

class Openh264Encoder;

class KSKinectDataEncoder
	: public Thread
{
public:
	typedef enum
	{
		SRC_TYPE_COLOR = 1,
		SRC_TYPE_DEPTH = 2,
		SRC_TYPE_SKELETON = 3,
	}eSrcType;
public:
	KSKinectDataEncoder(
		KSKinectDataSenderPtr sender,
		KSKinectDataEncoder::eSrcType type,
		const std::string& devicename);
	~KSKinectDataEncoder();

protected:
	void WorkingFunc() override;
	void Release();
	void OpenEncode(int w, int h);

	bool EncodeRgb(bool color_or_depth); //color true, depth false
	bool EncodeSkeleton();

private:
	const size_t MAX_QUEUE_SIZE = 10;

	KSKinectDataEncoder::eSrcType m_SrcType;
	std::string m_deviceName;

	KinectDataCaptureQueuePtr m_DataQueuePtr;
	KSKinectDataSenderPtr m_Sender;

	std::mutex m_EncodeMutex;
	Openh264Encoder* m_pEncoder;

	int m_lastW;
	int m_lastH;
};
