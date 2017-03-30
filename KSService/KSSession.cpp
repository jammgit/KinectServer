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
	, m_KDService(NULL)
	, m_Service(service)
	, IKSSession(sock)
{
}

KSSession::~KSSession()
{
	//���ﲻ�ܵ���Release,�ײ�AsyncTcpConnection����ʱ����
	//this->Release();
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
	KSLogService::GetInstance()->OutputMessage("Construct pack success\n");
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
		default:
			KSLogService::GetInstance()->OutputMessage("δ֪��ָ�����ͣ������Э��ͷ\n");
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
		KSLogService::GetInstance()->OutputMessage("���յ�[��������]Э��\n");
		break;
	}
	case IKSSession::CMD_NUM_CLI_UNCONN_REQ: {
		this->ReqUnConnect(frame);
		KSLogService::GetInstance()->OutputMessage("���յ�[�Ͽ���������]Э��\n");
		break;
	}
	case IKSSession::CMD_NUM_CLI_DEVICES_REQ: {
		this->ReqDeviceList(frame);
		KSLogService::GetInstance()->OutputMessage("���յ�[�����豸�б�]Э��\n");
		break;
	}
	default:
		KSLogService::GetInstance()->OutputMessage("δ֪��ָ������Э��ͷ\n");
		break;
	}
}

void KSSession::ReqConnect(const ShareFrame& frame)
{
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

		//ע����Ϣ�����ݷ�����
		m_Service->GetColorServerPtr()->RegisterCmdSock(m_StrGuid, shared_from_this());
		m_Service->GetDepthServerPtr()->RegisterCmdSock(m_StrGuid, shared_from_this());
		m_Service->GetSkeleServerPtr()->RegisterCmdSock(m_StrGuid, shared_from_this());
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
	this->ReleaseDataServerSource();

	if (m_Service)
		m_Service->ReleaseSession(m_StrGuid);
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

void KSSession::Release()
{
	this->ReleaseDataServerSource();

	AsyncTcpConnection::Release();

	if (m_Service)
		m_Service->ReleaseSession(m_StrGuid);
}

void KSSession::ReleaseDataServerSource()
{	//ɾ�����ݷ���������Ϣ�����ͷ�������˿����׽��ֹ�������Դ


}
