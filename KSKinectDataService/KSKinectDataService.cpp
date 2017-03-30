#include "KSKinectDataService.h"
#include "KinectDataProto.pb.h"
#include "../KSService/IKSSession.h"
#include "../KSLogService/KSLogService.h"
#include "../KSUtils/ShareFrame.h"
#include "../IKSKinectDataCapture/KinectDataCapturer.h"
#include "../KSUtils/CharsetUtils.h"
#include "../KSUtils/PortsMacro.h"
#include "../KSService/IKSService.h"
#include "../KSKinectDataServer/KSKinectDataServer.h"

KSKinectDataService::KSKinectDataService(
	IKSServicePtr service, IKSSessionPtr session)
	: m_Session(session)
	, m_Service(service)
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



void KSKinectDataService::ProcessStartReq(const ShareFrame& frame)
{//获取设备并回复状态
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
		respStart.set_devicename(name);
		if (!found)
		{
			respStart.set_resulttype(FAIL);
			respStart.set_failreason("no such device");
		}

		std::string data;
		respStart.SerializeToString(&data);

		ShareFrame frame = FrameBuffer::Make(
			data,
			IKSSession::CMD_TYPE_KINECT,
			IKSKinectDataService::CMD_NUM_SVR_START_RESP,
			frame->m_version,
			frame->m_u32Sequence);

		if (m_Session) m_Session->SendShareFrame(frame);
	}
}

void KSKinectDataService::ProcessEndReq(const ShareFrame& frame)
{//客户端请求停止发送数据
	KinectDataProto::pbReqEnd end;
	end.devicename();
	m_Session->StrGuid();
	

}

void KSKinectDataService::SendEnd(eSvrEndType type, const std::string& devname)
{//主动停止发送
	KinectDataProto::pbEndTransfer end;
	end.set_devicename(devname);
	end.set_type(type);
	end.set_reason("Close");
	
	std::string data;
	end.SerializeToString(&data);
	ShareFrame frame = FrameBuffer::Make(
		data,
		IKSSession::CMD_TYPE_KINECT,
		IKSKinectDataService::CMD_NUM_SVR_END_REQ,
		'0',
		0);

	if (m_Session) m_Session->SendShareFrame(frame);
}