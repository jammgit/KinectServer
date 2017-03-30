#include "KSKinectDataSender.h"
#include "KSKinectDataServer.h"
#include "DataChannelProto.pb.h"
#include "../KSUtils/ShareData.h"
#include "../KSService/KSSession.h"
#include "../KSKinectDataService/IKSKinectDataService.h"

KSKinectDataSender::KSKinectDataSender(
	AsyncTcpServerPtr server,
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



void KSKinectDataSender::Close()
{
	AsyncTcpConnection::Release();

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
	std::vector<ShareFrame> frames;
	m_ProtoProcess.TryParse(data, frames);
	for each(auto frame in frames)
	{
		DataChannelProto::pbDataChannelPkg pkg;
		if (pkg.ParseFromArray(frame->m_data, frame->m_u32length))
		{
			if ((frame->m_cmdType & 0xFF) == CMD_TYPE_DATA_CHANNEL
				&& (frame->m_cmdNum & 0xFF) == CMD_NUM_DATA_CHANNEL
				&& !m_EncoderPtr)
			{
				m_StrGuid = pkg.guid();
				m_DevName = pkg.devicename();
				// 注册信息
				KSKinectDataServer* server = NULL;
				if (m_pServer
					&& (server = static_cast<KSKinectDataServer*>(m_pServer.get())))
				{
					server->RegisterDataSock(
						pkg.guid(),
						pkg.devicename(),
						shared_from_this());
				}

				//分配编码线程
				std::lock_guard<std::mutex> lock(m_EncoderMutex);
				m_EncoderPtr = boost::make_shared<KSKinectDataEncoder>(
					shared_from_this(), m_type, m_DevName);

				if (m_EncoderPtr) m_EncoderPtr->Start();


			}
		}

	}
}

void KSKinectDataSender::DeviceUnlink()
{
	this->Close();

	std::lock_guard<std::mutex> lock(m_ReleaseMutex);
	if (!m_StrGuid.empty())
	{//有可能在没收到指令前就断开了socket
		KSKinectDataServer* server = NULL;
		if (m_pServer
			&& (server = static_cast<KSKinectDataServer*>(m_pServer.get())))
		{
			AsyncTcpConnectionPtr cmdSock;
			KSSession *session;
			if (server->GetCmdSock(m_StrGuid, cmdSock))
			{
				session = static_cast<KSSession*>(cmdSock.get());
				IKSKinectDataServicePtr kdService
					= session->KinectDataService();
				if (kdService)
					kdService->SendEnd(
						IKSKinectDataService::ERR_DEVICE,
						this->m_DevName);

				m_StrGuid.clear();
			}

		}
	}
}

void KSKinectDataSender::Release()
{ // 底层socket断开

	this->Close();

	std::lock_guard<std::mutex> lock(m_ReleaseMutex);
	if (!m_StrGuid.empty())
	{//有可能在没收到指令前就断开了socket
		KSKinectDataServer* server = NULL;
		if (m_pServer
			&& (server = static_cast<KSKinectDataServer*>(m_pServer.get())))
		{
			AsyncTcpConnectionPtr cmdSock;
			KSSession *session;
			if (server->GetCmdSock(m_StrGuid, cmdSock))
			{
				session = static_cast<KSSession*>(cmdSock.get());
				IKSKinectDataServicePtr kdService
					= session->KinectDataService();
				if (kdService)
					kdService->SendEnd(
						IKSKinectDataService::ERR_SOCKET,
						this->m_DevName);

				m_StrGuid.clear();
			}
		}
	}

}

void KSKinectDataSender::EndProcess()
{

}
