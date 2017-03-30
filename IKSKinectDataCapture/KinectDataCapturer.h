#pragma once

#include "KinectDeviceInfo.h"
#include "KinectDataCaptureQueue.h"
#include <assert.h>
#include <map>
#include <mutex>
#include "../KSUtils/Singleton.h"

//�豸��->���ݶ���
typedef std::map<std::wstring, KinectDataCaptureQueuePtr> DevName2DataQueueMap;
//�豸��->���豸����������Ϣ
typedef std::map<std::wstring, KinectDeviceInfoPtr> DevName2DevInfoMap;
typedef std::list<std::wstring> DeviceNameList;

class KinectDataCapturer
	: public Singleton<KinectDataCapturer>
{
public:
	KinectDataCapturer();
	~KinectDataCapturer();

	void RegisterDevStatusCallBack(); //��ʼ�豸���ݻ�ȡ

	void GetDeviceList(DeviceNameList& devList);

	bool GetDataQueue(
		const std::wstring& devicename,
		KinectDataCaptureQueuePtr &observer);

protected:
	HRESULT NuiInit(KinectDeviceInfoPtr& device);
	//HRESULT NuiInit(BSTR instanceName);
	void NuiUnInit(const wchar_t* uniqueName);
	//void NuiZero();

	void NuiGotDepthAlert(KinectDeviceInfoPtr& device);
	void NuiGotColorAlert(KinectDeviceInfoPtr& device);
	void NuiGotSkeletonAlert(KinectDeviceInfoPtr& device);
	RGBQUAD NuiShortToQuadDepth(USHORT s);

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
