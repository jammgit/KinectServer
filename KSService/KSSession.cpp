#include "KSSession.h"
#include "IKSService.h"
#include "ConnectProto.pb.h"
#include <vector>
#include <string>

#include "../KSKinectDataService/KSKinectDataService.h"
#include "../KSKinectDataServer/KSKinectDataServer.h"
#include "../IKSKinectDataCapture/KinectDataCapturer.h"
#include "../KSLogService/KSLogService.h"
#include "../KSUtils/CharsetUtils.h"

KSSession::KSSession(IKSServicePtr service, socket_ptr sock, std::string guid)
	: m_StrGuid(guid)
	, m_bRecvPing(false)
	, m_KDService(NULL)
	, m_Service(service)
	, IKSSession(sock)
{
}

KSSession::~KSSession()
{
	this->Release();
}

void KSSession::RegisterAllService()
{
	if (!m_KDService) 
		m_KDService = boost::make_shared<KSKinectDataService>(m_Service, shared_from_this());
}

IKSKinectDataServicePtr KSSession::KinectDataService()
{
	return m_KDService;
}

std::string KSSession::StrGuid()
{
	return m_StrGuid;
}

void KSSession::SendShareFrame(ShareFrame frame)
{
	ShareData data;
	m_protocProc.TryPack(frame, data);
	if (NULL != data) this->SendShareData(data);
}

void KSSession::TryParse(const ShareData& data)
{
	std::vector<ShareFrame> frames;
	m_protocProc.TryParse(data, frames);
	for each(auto frame in frames)
	{
		switch (frame->m_cmdType & 0xFF)
		{
		case IKSSession::CMD_TYPE_CONNECT: {
			this->DoFrame(frame);
			break;
		}
		case IKSSession::CMD_TYPE_KINECT: {
			if (m_KDService)
				m_KDService->DoFrame(frame);
			break;
		}
		case IKSSession::CMD_TYPE_PING: {
			this->DoPing(frame);
			break;
		}
		default:
			KSLogService::GetInstance()->OutputMessage("未知的指令类型，错误的协议头\n");
			break;
		}
	}
}

void KSSession::DoFrame(const ShareFrame& frame)
{
	switch (frame->m_cmdNum & 0xFF)
	{
	case IKSSession::CMD_NUM_CLI_CONN_REQ: {
		this->ReqConnect(frame);
		KSLogService::GetInstance()->OutputMessage("接收到[连接请求]协议\n");
		break;
	}
	case IKSSession::CMD_NUM_CLI_UNCONN_REQ: {
		this->ReqUnConnect(frame);
		KSLogService::GetInstance()->OutputMessage("接收到[断开连接请求]协议\n");
		break;
	}
	case IKSSession::CMD_NUM_CLI_DEVICES_REQ: {
		this->ReqDeviceList(frame);
		KSLogService::GetInstance()->OutputMessage("接收到[请求设备列表]协议\n");
		break;
	}
	default:
		KSLogService::GetInstance()->OutputMessage("未知的指令，错误的协议头\n");
		break;
	}
}

void KSSession::ReqConnect(const ShareFrame& frame)
{
	ConnectProto::pbReqConnect reqConn;
	if (!reqConn.ParseFromArray(frame->m_data, frame->m_u32length))
		return;
	m_PeerHostname = reqConn.hostname();
	KSLogService::GetInstance()->OutputClient(reqConn.hostname(), true);

	ConnectProto::pbRespConnect rConn;
	std::string tmp("connect success");
	rConn.set_resulttype(IKSSession::SUCCESS);
	rConn.set_failreason(tmp);
	rConn.set_guid(m_StrGuid);
	if (m_Service)
	{
		rConn.set_colorport(m_Service->GetColorServerPtr()->GetPort());
		rConn.set_depthport(m_Service->GetDepthServerPtr()->GetPort());
		rConn.set_skeleport(m_Service->GetSkeleServerPtr()->GetPort());
	}
	std::string str;
	rConn.SerializeToString(&str);
	ShareFrame sframe = FrameBuffer::Make(
		str,
		frame->m_cmdType,
		IKSSession::CMD_NUM_SVR_CONN_RESP,
		frame->m_version,
		frame->m_u32Sequence);
	this->SendShareFrame(sframe);
}

void KSSession::ReqUnConnect(const ShareFrame& frame)
{
	this->Release();
}

void KSSession::ReqDeviceList(const ShareFrame& frame)
{
	DeviceNameList nameList;
	KinectDataCapturer::GetInstance()->GetDeviceList(nameList);
	auto iter = nameList.begin();
	std::string names;
	for (; iter != nameList.end();)
	{
		std::string name;
		CharsetUtils::UnicodeStringToANSIString(*iter++, name);
		names.append(name);
		if (iter != nameList.end())
			names.append(";");
	}

	ConnectProto::pbRespDevices rDevices;
	rDevices.set_devicelist(names.c_str());
	rDevices.set_separator(";");
	std::string str;
	rDevices.SerializeToString(&str);
	ShareFrame sframe = FrameBuffer::Make(
		str,
		frame->m_cmdType,
		IKSSession::CMD_NUM_SVR_DEVICES_RESP,
		frame->m_version,
		frame->m_u32Sequence);
	this->SendShareFrame(sframe);
}

void KSSession::DoPing(ShareFrame frame)
{
	switch (frame->m_cmdNum & 0xFF)
	{
	case IKSSession::CMD_NUM_CLI_PING: {
		m_bRecvPing = true;
		frame->m_cmdNum = IKSSession::CMD_NUM_SVR_PING;
		this->SendShareFrame(frame);
		KSLogService::GetInstance()->OutputMessage("接收到[套接字保活]协议\n");
		break;
	}
	default:
		break;
	}
}

void KSSession::KeepAliveHandler(const boost::system::error_code &err)
{
	if (err) return;
	if (m_bRecvPing)
	{
		m_bRecvPing = false;
		return;
	}

	this->Release();
}

void KSSession::Release()
{
	IKSSession::Release();

	KSLogService::GetInstance()->OutputClient(m_PeerHostname, false);

	if (m_Service)
		m_Service->ReleaseSession(m_StrGuid);
}


