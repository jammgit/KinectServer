#include "KSSession.h"
#include "KSService.h"
#include "ConnectProto.pb.h"
#include <vector>
#include <string>

#include "../KSKinectDataService/KSKinectDataService.h"
#include "../IKSKinectDataCapture/KinectDataCapturer.h"
#include "../KSLogService/KSLogService.h"
#include "../KSUtils/CharsetUtils.h"

KSSession::KSSession(IKSService* service, socket_ptr sock)
	: m_pKDService(NULL)
	, m_pService(service)
	, AsyncTcpConnection(sock)
{
}

KSSession::~KSSession()
{
	//���ﲻ�ܵ���Release,�ײ�AsyncTcpConnection����ʱ����
	//this->Release();
}

void KSSession::RegisterAllService()
{
	if (!m_pKDService) m_pKDService = new KSKinectDataService(m_pService, this);
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
			if (m_pKDService)
				m_pKDService->DoFrame(frame);

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
		ConnectProto::pbRespConnect rConn;
		std::string tmp("connect success");
		rConn.set_resulttype(IKSSession::SUCCESS);
		rConn.set_failreason(tmp);
		std::string str;
		rConn.SerializeToString(&str);
		ShareFrame sframe = FrameBuffer::Make(
			str,
			frame->m_cmdType,
			IKSSession::CMD_NUM_SVR_CONN_RESP,
			frame->m_version,
			frame->m_u32Sequence);
		this->SendShareFrame(sframe);
		KSLogService::GetInstance()->OutputMessage("���յ�[��������]Э��\n");
		break;
	}
	case IKSSession::CMD_NUM_CLI_UNCONN_REQ: {
		if (m_pService)
			m_pService->ReleaseSession(shared_from_this());
		KSLogService::GetInstance()->OutputMessage("���յ�[�Ͽ���������]Э��\n");
		break;
	}
	case IKSSession::CMD_NUM_CLI_DEVICES_REQ: {
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
		KSLogService::GetInstance()->OutputMessage("���յ�[�����豸�б�]Э��\n");
		break;
	}
	default:
		KSLogService::GetInstance()->OutputMessage("δ֪��ָ������Э��ͷ\n");
		break;
	}
}

void KSSession::Release()
{
	AsyncTcpConnection::Release();

	if (m_pKDService) { delete m_pKDService; m_pKDService = NULL; }

	if (m_pService)
		m_pService->ReleaseSession(shared_from_this());
}