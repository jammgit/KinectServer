#include "KSKinectDataService.h"
#include "KinectDataProto.pb.h"
#include "../KSService/KSSession.h"
#include "../KSLogService/KSLogService.h"
#include "../KSUtils/ShareFrame.h"
#include "../IKSKinectDataCapture/KinectDataCapturer.h"
#include "../KSUtils/CharsetUtils.h"
#include "../KSUtils/PortsMacro.h"
#include "../KSService/IKSService.h"
#include "KSKinectDataServer.h"

KSKinectDataService::KSKinectDataService(IKSService* service, KSSession *session)
	: m_pSession(session)
	, m_pService(service)
{
}

KSKinectDataService::~KSKinectDataService()
{
}

void KSKinectDataService::DoFrame(const ShareFrame& frame)
{
	switch (frame->m_cmdNum & 0xFF)
	{
	case KSKinectDataService::CMD_NUM_CLI_START_REQ: {
		this->ProcessStartReq(frame);
		KSLogService::GetInstance()->OutputMessage("接收到[开始请求Kinect]数据协议\n");
		break;
	}
	case KSKinectDataService::CMD_NUM_CLI_END_REQ: {
		this->ProcessEndReq(frame);
		KSLogService::GetInstance()->OutputMessage("接收到[结束请求Kinect]数据协议\n");
		break;
	}
	default:
		KSLogService::GetInstance()->OutputMessage("未知的指令号，错误的协议头\n");
		break;
	}
}

void KSKinectDataService::SendEnd()
{
	//主动停止发送
	std::string data;
	ShareFrame frame = FrameBuffer::Make(
		data,
		IKSSession::CMD_TYPE_KINECT,
		IKSKinectDataService::CMD_NUM_SVR_END_REQ,
		'0',
		0);

	if (m_pSession) m_pSession->SendShareFrame(frame);
}

void KSKinectDataService::ProcessStartReq(const ShareFrame& frame)
{
	KinectDataProto::pbReqStart start;
	if (start.ParseFromArray(frame->m_data, frame->m_u32length))
	{
		std::string name = start.devicename();
		std::wstring wname;
		CharsetUtils::ANSIStringToUnicodeString(name, wname);
		DeviceNameList names;
		KinectDataCapturer::GetInstance()->GetDeviceList(names);
		bool found = false;
		auto iter = names.begin();
		for (; iter != names.end(); ++iter)
		{
			if (wcsncmp(wname.c_str(), iter->c_str(), iter->length()) == 0)
			{
				found = true;
				break;
			}
		}

		KinectDataProto::pbRespStart respStart;
		respStart.set_resulttype(SUCCESS);
		respStart.set_failreason("Success");
		if (!found)
		{
			respStart.set_resulttype(FAIL);
			respStart.set_failreason("no such device");
		}
		else
		{
			respStart.set_colorport(m_pService->GetColorPort());
			respStart.set_depthport(m_pService->GetDepthPort());
			respStart.set_skeleport(m_pService->GetSkeletonPort());

			//将关系记录到Server
			Addresses addrs;
			addrs.addr[0] = { m_pSession->GetAddrPort().addr, (unsigned short)start.colorport() };
			addrs.addr[1] = { m_pSession->GetAddrPort().addr, (unsigned short)start.depthport() };
			addrs.addr[2] = { m_pSession->GetAddrPort().addr, (unsigned short)start.skeleport() };
			KSKinectDataServer::SetDataAddrByCmdAddr(m_pSession->GetAddrPort(), addrs);
			KSKinectDataServer::SetSockByCmdAddr(m_pSession->GetAddrPort(), m_pSession->shared_from_this());
		}
		std::string data;
		respStart.SerializeToString(&data);

		ShareFrame frame = FrameBuffer::Make(
			data,
			IKSSession::CMD_TYPE_KINECT,
			IKSKinectDataService::CMD_NUM_SVR_START_RESP,
			frame->m_version,
			frame->m_u32Sequence);

		if (m_pSession) m_pSession->SendShareFrame(frame);
	}
}

void KSKinectDataService::ProcessEndReq(const ShareFrame& frame)
{
	//停止发送数据
	Addresses addrs;
	if (KSKinectDataServer::ResetDataAddrByCmdAddr(
		m_pSession->GetAddrPort(), addrs))
	{
		KSKinectDataServer::CloseSenders(addrs);
	}
}