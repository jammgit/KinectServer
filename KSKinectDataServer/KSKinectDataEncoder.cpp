#include "KSKinectDataEncoder.h"
#include "KSKinectDataSender.h"
//#include "../KSUtils/AsyncTcpConnection.h"
#include "../IKSKineckH264Encode/Openh264Encoder.h"
#include "../IKSKinectDataCapture/KinectDataCapturer.h"
#include "../KSUtils/CharsetUtils.h"
#include "../include/yuv/libyuv.h"

#include "../IKSKinectDataCapture/EncodedColorFrame.h"
#include "../IKSKinectDataCapture/EncodedDepthFrame.h"
#include "../IKSKinectDataCapture/SkeletonFrame.h"

#include <stdio.h>

KSKinectDataEncoder::KSKinectDataEncoder(
	KSKinectDataSenderPtr sender,
	KSKinectDataEncoder::eSrcType type,
	const std::string& devicename)
	: m_SrcType(type)
	, m_deviceName(devicename)
	, m_Sender(sender)
	, m_pEncoder(NULL)
{
	m_lastH = 0;
	m_lastW = 0;
}

KSKinectDataEncoder::~KSKinectDataEncoder()
{
	this->Release();
}

void KSKinectDataEncoder::WorkingFunc()
{
	bool kOff = false;

	switch (m_SrcType)
	{
	case KSKinectDataEncoder::SRC_TYPE_COLOR: {
		kOff = this->EncodeRgb(true);
		break;
	}
	case KSKinectDataEncoder::SRC_TYPE_DEPTH: {
		kOff = this->EncodeRgb(false);
		break;
	}
	case KSKinectDataEncoder::SRC_TYPE_SKELETON: {
		kOff = this->EncodeSkeleton();
		break;
	}
	default:
		break;
	}
	m_bWorkingSwitch = false;
}


bool KSKinectDataEncoder::EncodeRgb(bool color_or_depth)
{//编码为二进制，发送
	uint8_t *yuvBuf = NULL;
	uint8_t *y = NULL;
	uint8_t *u = NULL;
	uint8_t *v = NULL;
	uint8_t *in_data[3] = { 0 };
	int in_linesize[3];

	uint8_t *out_data[8];
	size_t out_linesize[8];

	E_ISWH264ENC_RET ret;

	auto initBufFunc = [&](int w, int h)
	{
		yuvBuf = new uint8_t[w*h * 3 / 2];
		y = yuvBuf;
		u = yuvBuf + w*h;
		v = u + w*h / 4;
		in_data[0] = y;
		in_data[1] = u;
		in_data[2] = v;
		in_linesize[0] = w;
		in_linesize[1] = in_linesize[2] = (w + 1) / 2;
	};
	auto releaseBufFunc = [&]()
	{
		if (yuvBuf)
		{
			delete [] yuvBuf;
			yuvBuf = NULL;
		}
	};

	std::wstring wname;
	KinectDataCaptureQueuePtr queueptr;
	CharsetUtils::ANSIStringToUnicodeString(m_deviceName, wname);
	KinectDataCapturer::GetInstance()->GetDataQueue(wname, queueptr);
	unsigned int lastFrameNum = 0;

	while (queueptr && m_bWorkingSwitch)
	{
		if (m_Sender->GetDataQueueSize() >= MAX_QUEUE_SIZE)
		{
			Sleep(25);
			continue;
		}
		// get one frame
		MiddleRgbFramePtr frame = MiddleRgbFrame::Make(0,0,0,NULL);
		frame->m_frameNumber = lastFrameNum;
		bool got;
		if (color_or_depth) got = queueptr->GetColorFrame(frame);
		else got = queueptr->GetDepthFrame(frame);
	
		if (!got)
		{
			//whether device unlink
			if (queueptr->IsStopProvideData())
			{
				releaseBufFunc();
				m_Sender->DeviceUnlink();
				return false; //kinect 断开
			}
			Sleep(25);
			continue;
		}

		lastFrameNum = frame->m_frameNumber;

		if (frame->m_w != m_lastW || frame->m_h != m_lastH)
		{
			this->OpenEncode(frame->m_w, frame->m_h);
			releaseBufFunc();
			initBufFunc(frame->m_w, frame->m_h);
		}
		m_lastW = frame->m_w;
		m_lastH = frame->m_h;

		libyuv::ARGBToI420(frame->m_rgb,
			frame->m_w * 4,
			y, in_linesize[0],
			u, in_linesize[1],
			v, in_linesize[2],
			frame->m_w,
			frame->m_h);

		ret = ERR_ENCODER_NOT_OPENED;
		{
			std::lock_guard<std::mutex> lock(m_EncodeMutex);
			if (m_pEncoder)
				ret = m_pEncoder->RequestEncodedFrame(in_data, out_data, out_linesize);
		}
		if (ret == ERR_NONE)
		{
			for (int i = 0; out_data[i]; i++)
			{
				Middle264FramePtr encodedFrame = EncodedColorFrame::Make(
					frame->m_frameNumber,
					KSKinectDataEncoder::SRC_TYPE_COLOR,
					out_linesize[i],
					out_data[i]);

				m_Sender->Send264Frame(encodedFrame);
			}
		}
	}
	releaseBufFunc();
	return true;
}

bool KSKinectDataEncoder::EncodeSkeleton()
{
	std::wstring wname;
	KinectDataCaptureQueuePtr queueptr;
	CharsetUtils::ANSIStringToUnicodeString(m_deviceName, wname);
	KinectDataCapturer::GetInstance()->GetDataQueue(wname, queueptr);
	//	bool kinectOff;

	//while (queueptr && m_bWorkingSwitch)
	//{
	//	SkeletonFramePtr frame;
	//	if (!queueptr->GetSkeleFrame(frame))
	//	{
	//		bool b = queueptr->IsStopProvideData();
	//		if (b) return false; //kinect 断开
	//	}

	//	m_Sender->SendSkeletonFrame(frame);
	//}

	return true;
}

void KSKinectDataEncoder::OpenEncode(int w, int h)
{
	std::lock_guard<std::mutex> lock(m_EncodeMutex);
	if (m_pEncoder)
	{
		m_pEncoder->Close();
		delete m_pEncoder;
		m_pEncoder = NULL;
	}
	m_pEncoder = new Openh264Encoder;
	S_ISWH264EncoderParam param;
	param.ePreset = PRE_DEFAULT;
	param.eProfile = PROFILE_MAIN;
	param.eRcMode = RC_CQP;
	param.eTune = TUNE_DEFAULT;
	param.iBitrate = 640000;
	param.iFrameRate = 25;
	param.iHeight = h;
	param.iWidth = w;
	E_ISWH264ENC_RET ret = m_pEncoder->SetParam(param);
	ret = m_pEncoder->Open();
}

void KSKinectDataEncoder::Release()
{
	std::lock_guard<std::mutex> lock(m_EncodeMutex);
	if (m_pEncoder)
	{
		m_pEncoder->Close();
		delete m_pEncoder;
		m_pEncoder = NULL;
	}
}