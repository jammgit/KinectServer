#pragma once

#include <Windows.h>
#include <boost\shared_ptr.hpp>
#include <string>
struct INuiSensor;

class KinectDeviceInfo;
typedef boost::shared_ptr<KinectDeviceInfo> KinectDeviceInfoPtr;

class KinectDataCapturer;

class KinectDeviceInfo
{
public:
	KinectDeviceInfo()
		: m_pKinectDataCapturer(NULL)
		, m_pNuiSensor(NULL)
		, m_hThNuiProcess(NULL)
		, m_hEvNuiProcessStop(NULL)
		, m_hNextColorFrameEvent(NULL)
		, m_hNextDepthFrameEvent(NULL)
		, m_hNextSkeletonEvent(NULL)
		, m_pVideoStreamHandle(NULL)
		, m_pDepthStreamHandle(NULL)
		, m_bScreenBlanked(false)
		, m_bAppTracking(false)
	{}
	KinectDataCapturer *m_pKinectDataCapturer;
	// Current kinect
	INuiSensor *        m_pNuiSensor;
	std::wstring         m_instanceName;
	std::wstring			m_instanceId;
	std::wstring			m_deviceName;

	// 线程句柄
	HANDLE m_hThNuiProcess;

	// 线程通知事件
	HANDLE m_hEvNuiProcessStop;
	HANDLE m_hNextColorFrameEvent;
	HANDLE m_hNextDepthFrameEvent;
	HANDLE m_hNextSkeletonEvent;

	HANDLE m_pVideoStreamHandle;
	HANDLE m_pDepthStreamHandle;

	RGBQUAD m_rgbWk[640 * 480];

	bool m_bScreenBlanked;
	bool m_bAppTracking;
	DWORD m_SkeletonIds[6];
};
