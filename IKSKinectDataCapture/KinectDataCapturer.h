#pragma once

#include "KinectDeviceInfo.h"
#include "KinectDataCaptureQueue.h"
#include <assert.h>
#include <map>
#include <mutex>
#include "../KSUtils/Singleton.h"

class IKDCaptureClient;

//设备名->数据队列
typedef std::map<std::wstring, KinectDataCaptureQueuePtr> DevName2DataQueueMap;
//设备名->打开设备所产生的信息
typedef std::map<std::wstring, KinectDeviceInfoPtr> DevName2DevInfoMap;
typedef std::list<std::wstring> DeviceNameList;

typedef struct _NUI_SKELETON_DATA NUI_SKELETON_DATA;

class KinectDataCapturer
	: public Singleton<KinectDataCapturer>
{
public:
	KinectDataCapturer();
	~KinectDataCapturer();

	void RegisterClient(IKDCaptureClient *client);
	IKDCaptureClient* m_pClient;

	void RegisterDevStatusCallBack(); //开始设备数据获取

	void GetDeviceList(DeviceNameList& devList);

	bool GetDataQueue(
		const std::wstring& devicename,
		KinectDataCaptureQueuePtr &observer);

protected:
	HRESULT NuiInit(); // init call
	HRESULT NuiInit(KinectDeviceInfoPtr& device); //call back
	//HRESULT NuiInit(BSTR instanceName);
	void NuiUnInit(const wchar_t* uniqueName);
	//void NuiZero();

	void NuiGotDepthAlert(KinectDeviceInfoPtr& device);
	void NuiGotColorAlert(KinectDeviceInfoPtr& device);
	void NuiGotSkeletonAlert(KinectDeviceInfoPtr& device);
	RGBQUAD NuiShortToQuadDepth(USHORT s);

protected:
	void NuiDrawSkeletonSegment(SkeletonFramePtr &frame, NUI_SKELETON_DATA * pSkel, POINT points[20], int numJoints, ...);

protected:
	static DWORD __stdcall     NuiProcessThread(LPVOID pParam);
	DWORD __stdcall            NuiProcessThread(KinectDeviceInfoPtr infoPtr);

	static void __stdcall NuiStatusProcThunk(HRESULT hrStatus, const wchar_t* instanceName, const wchar_t* uniqueDeviceName, void* pUserData);
	void __stdcall NuiStatusProc(HRESULT hrStatus, const wchar_t* instanceName, const wchar_t* uniqueDeviceName);

private:
	DevName2DataQueueMap	m_DevName2DataQueueMap;
	DevName2DevInfoMap		m_DevName2DevInfoMap;

	std::mutex				m_ListMutex;
	DeviceNameList			m_DevNameList;

	std::mutex m_callBackMutex;
};
