#include "KSKinectDataSender.h"

#include "KSKinectDataServer.h"
#include "../KSUtils/ShareData.h"

KSKinectDataSender::KSKinectDataSender(
	KSKinectDataServer* server,
	socket_ptr sock,
	KSKinectDataEncoder::eSrcType type)
	: AsyncTcpConnection(sock)
	, m_EncoderPtr(NULL)
	, m_type(type)
	, m_pServer(server)
{
}

KSKinectDataSender::~KSKinectDataSender()
{
}

void KSKinectDataSender::Open()
{
	this->Close();

	m_EncoderPtr = boost::make_shared<KSKinectDataEncoder>(
		shared_from_this(), m_type, m_deviceName);

	if (m_EncoderPtr) m_EncoderPtr->Start();
}

void KSKinectDataSender::Close()
{
	std::lock_guard<std::mutex> lock(m_EncoderMutex);
	if (m_EncoderPtr)
	{
		m_EncoderPtr->Stop();
		m_EncoderPtr = NULL;
	}
}

void KSKinectDataSender::Send264Frame(Middle264FramePtr frame)
{
	ShareData head = DataBuffer::Make(12);
	head->m_data[0] = frame->m_FrameNumber[3];
	head->m_data[1] = frame->m_FrameNumber[2];
	head->m_data[2] = frame->m_FrameNumber[1];
	head->m_data[3] = frame->m_FrameNumber[0];

	head->m_data[4] = frame->m_FrameType[3];
	head->m_data[5] = frame->m_FrameType[2];
	head->m_data[6] = frame->m_FrameType[1];
	head->m_data[7] = frame->m_FrameType[0];

	head->m_data[8] = frame->m_Length[3];
	head->m_data[9] = frame->m_Length[2];
	head->m_data[10] = frame->m_Length[1];
	head->m_data[11] = frame->m_Length[0];
	SendShareData(head);

	ShareData data = DataBuffer::Make(0);
	data->m_used = 0;
	data->m_len = frame->m_u32Length;
	data->m_data = frame->m_Data;
	frame->m_Data = NULL; //pass memory

	SendShareData(data);
}

void KSKinectDataSender::SendSkeletonFrame(SkeletonFramePtr frame)
{
}

void KSKinectDataSender::TryParse(const ShareData& data)
{
}

void KSKinectDataSender::Release()
{
	AsyncTcpConnection::Release();

	this->Close();

	if (m_pServer) m_pServer->ReleaseSender(shared_from_this());
}