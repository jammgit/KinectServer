#include "KinectDataCapturer.h"
#include "KinectDataConst.h"
#include "../KSUtils/CharsetUtils.h"
#include "../KSLogService/KSLogService.h"

#include <NuiApi.h>

KinectDataCapturer::KinectDataCapturer()
{
}

KinectDataCapturer::~KinectDataCapturer()
{
}

void KinectDataCapturer::RegisterDevStatusCallBack()
{
	NuiSetDeviceStatusCallback(
		&KinectDataCapturer::NuiStatusProcThunk, this);
}

void KinectDataCapturer::GetDeviceList(DeviceNameList& devList)
{
	std::lock_guard<std::mutex> lock(m_ListMutex);
	devList = m_DevNameList;
}

bool KinectDataCapturer::GetDataQueue(
	const std::wstring& devicename,
	KinectDataCaptureQueuePtr &observer)
{
	auto iter = m_DevName2DataQueueMap.find(devicename);
	if (iter == m_DevName2DataQueueMap.end())
	{
		observer = NULL;
		return false;
	}
	observer = m_DevName2DataQueueMap[devicename];
	return true;
}

void CALLBACK KinectDataCapturer::NuiStatusProcThunk(HRESULT hrStatus, const wchar_t* instanceName, const wchar_t* uniqueDeviceName, void* pUserData)
{
	if (pUserData)
	{
		KinectDataCapturer* tmp = static_cast<KinectDataCapturer*>(pUserData);
		tmp->NuiStatusProc(hrStatus, instanceName, uniqueDeviceName);
	}
}

void CALLBACK KinectDataCapturer::NuiStatusProc(
	HRESULT hrStatus,
	const wchar_t* instanceName,
	const wchar_t* uniqueDeviceName)
{
	//std::lock_guard<std::mutex> lock(m_callBackMutex);

	if (SUCCEEDED(hrStatus))
	{//连上
		if (S_OK == hrStatus)
		{
			KinectDeviceInfoPtr devInfo = boost::make_shared<KinectDeviceInfo>();
			devInfo->m_instanceName = instanceName;
			devInfo->m_deviceName = uniqueDeviceName;
			//创建map
			m_DevName2DevInfoMap[devInfo->m_instanceName] = devInfo;
			m_DevName2DevInfoMap[devInfo->m_instanceName]->m_pKinectDataCapturer = this;
			//m_DevName2DataQueueMap[devInfo->m_deviceName] =
			//	boost::make_shared<KinectDataCaptureQueue>();

			HRESULT ret = this->NuiInit(devInfo);
			if (FAILED(ret))
			{
				m_DevName2DevInfoMap[devInfo->m_instanceName] = NULL;
				return;
			}
			m_DevName2DataQueueMap[devInfo->m_instanceName] =
				boost::make_shared<KinectDataCaptureQueue>();

			std::lock_guard<std::mutex> lock(m_ListMutex);
			m_DevNameList.push_back(instanceName);

			std::wstring w(instanceName);
			std::string a;
			CharsetUtils::UnicodeStringToANSIString(w, a);
			KSLogService::GetInstance()->OutputDevice(a.c_str(), true);
		}
	}
	else
	{//断开连接
		if (instanceName == NULL) return;

		auto mapiter = m_DevName2DevInfoMap.find(instanceName);
		if (mapiter == m_DevName2DevInfoMap.end())
			return;
		NuiUnInit(instanceName);

		m_DevName2DevInfoMap.erase(mapiter);

		auto datamapiter = m_DevName2DataQueueMap.find(instanceName);
		if (datamapiter == m_DevName2DataQueueMap.end())
			return;
		m_DevName2DataQueueMap.erase(datamapiter);

		std::lock_guard<std::mutex> lock(m_ListMutex);
		auto iter = m_DevNameList.begin();
		for each(auto elem in m_DevNameList)
		{
			if (wcsncmp(elem.c_str(), instanceName, elem.length()))
			{
				m_DevNameList.erase(iter);
				break;
			}
			++iter;
		}

		std::wstring w(instanceName);
		std::string a;
		CharsetUtils::UnicodeStringToANSIString(w, a);
		KSLogService::GetInstance()->OutputDevice(a.c_str(), false);
	}
}

HRESULT KinectDataCapturer::NuiInit(KinectDeviceInfoPtr& device)
{
	if (device->m_instanceName.empty())
	{
		return E_FAIL;
	}

	HRESULT hr = NuiCreateSensorById(device->m_instanceName.c_str(), &device->m_pNuiSensor);

	// Generic creation failure
	if (FAILED(hr))
	{
		return hr;
	}

	//SysFreeString(m_instanceId);

	device->m_instanceId = device->m_pNuiSensor->NuiDeviceConnectionId();

	device->m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	device->m_hNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	device->m_hNextSkeletonEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// 初始化三种数据源
	DWORD nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX
		| NUI_INITIALIZE_FLAG_USES_SKELETON
		| NUI_INITIALIZE_FLAG_USES_COLOR;
	hr = device->m_pNuiSensor->NuiInitialize(nuiFlags);
	if (E_NUI_SKELETAL_ENGINE_BUSY == hr)
	{
		nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR;
		hr = device->m_pNuiSensor->NuiInitialize(nuiFlags);
	}

	if (FAILED(hr))
		return hr;

	if (HasSkeletalEngine(device->m_pNuiSensor))
	{
		hr = device->m_pNuiSensor->NuiSkeletonTrackingEnable(device->m_hNextSkeletonEvent, 0);
		if (FAILED(hr))
			return hr;
	}

	hr = device->m_pNuiSensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_COLOR,
		NUI_IMAGE_RESOLUTION_640x480,
		0,
		2,
		device->m_hNextColorFrameEvent,
		&device->m_pVideoStreamHandle);

	if (FAILED(hr))
		return hr;

	hr = device->m_pNuiSensor->NuiImageStreamOpen(
		HasSkeletalEngine(device->m_pNuiSensor) ?
		NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH,
		NUI_IMAGE_RESOLUTION_320x240,
		0,
		2,
		device->m_hNextDepthFrameEvent,
		&device->m_pDepthStreamHandle);

	if (FAILED(hr))
		return hr;

	// Start the Nui processing thread
	device->m_hEvNuiProcessStop = CreateEvent(NULL, FALSE, FALSE, NULL);
	device->m_hThNuiProcess = CreateThread(NULL, 0, NuiProcessThread, &device/*this*/, 0, NULL);

	//创建队列
	m_DevName2DataQueueMap[device->m_deviceName] =
		boost::make_shared<KinectDataCaptureQueue>();

	return hr;
}

DWORD WINAPI KinectDataCapturer::NuiProcessThread(LPVOID pParam)
{
	KinectDeviceInfoPtr *pthis = (KinectDeviceInfoPtr *)pParam;
	return (*pthis)->m_pKinectDataCapturer->NuiProcessThread(*pthis);
}

DWORD WINAPI KinectDataCapturer::NuiProcessThread(KinectDeviceInfoPtr infoPtr)
{
	const int numEvents = 4;
	HANDLE hEvents[numEvents] = {
		infoPtr->m_hEvNuiProcessStop,
		infoPtr->m_hNextDepthFrameEvent,
		infoPtr->m_hNextColorFrameEvent,
		infoPtr->m_hNextSkeletonEvent };
	int    nEventIdx;
	//	DWORD  t;

		// Main thread loop
	bool continueProcessing = true;
	while (continueProcessing)
	{
		// Wait for any of the events to be signalled
		nEventIdx = WaitForMultipleObjects(numEvents, hEvents, FALSE, 100);

		// Process signal events
		switch (nEventIdx)
		{
		case WAIT_TIMEOUT:
			continue;

			// If the stop event, stop looping and exit
		case WAIT_OBJECT_0:
			continueProcessing = false;
			continue;

		case WAIT_OBJECT_0 + 1:
			NuiGotDepthAlert(infoPtr);
			break;

		case WAIT_OBJECT_0 + 2:
			NuiGotColorAlert(infoPtr);
			break;

		case WAIT_OBJECT_0 + 3:
			NuiGotSkeletonAlert(infoPtr);
			break;
		}
	}

	return 0;
}

void KinectDataCapturer::NuiGotDepthAlert(KinectDeviceInfoPtr& device)
{
	NUI_IMAGE_FRAME imageFrame;

	HRESULT hr = device->m_pNuiSensor->NuiImageStreamGetNextFrame(
		device->m_pDepthStreamHandle,
		0,
		&imageFrame);

	if (FAILED(hr))
	{
		return;
	}

	INuiFrameTexture * pTexture = imageFrame.pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);
	if (0 != LockedRect.Pitch)
	{
		DWORD frameWidth, frameHeight;

		NuiImageResolutionToSize(imageFrame.eResolution, frameWidth, frameHeight);

		// draw the bits to the bitmap
		RGBQUAD * rgbrun = device->m_rgbWk;
		USHORT * pBufferRun = (USHORT *)LockedRect.pBits;

		// end pixel is start + width*height - 1
		USHORT * pBufferEnd = pBufferRun + (frameWidth * frameHeight);

		assert(frameWidth * frameHeight <= ARRAYSIZE(device->m_rgbWk));

		while (pBufferRun < pBufferEnd)
		{
			*rgbrun = NuiShortToQuadDepth(*pBufferRun);
			++pBufferRun;
			++rgbrun;
		}

		MiddleRgbFramePtr frame = DepthFrame::Make(
			(unsigned char*)device->m_rgbWk,
			frameWidth * frameHeight * 4,
			frameWidth,
			frameHeight);

		frame->m_frameNumber = imageFrame.dwFrameNumber;
		m_DevName2DataQueueMap[device->m_instanceName]->RenewDepthFrame(frame);

		//m_pDrawDepth->Draw((BYTE*)m_rgbWk, frameWidth * frameHeight * 4);
	}
	else
	{
		OutputDebugString(L"Buffer length of received texture is bogus\r\n");
	}

	pTexture->UnlockRect(0);

	device->m_pNuiSensor->NuiImageStreamReleaseFrame(device->m_pDepthStreamHandle, &imageFrame);
}

void KinectDataCapturer::NuiGotColorAlert(KinectDeviceInfoPtr& device)
{
	NUI_IMAGE_FRAME imageFrame;

	HRESULT hr = device->m_pNuiSensor->NuiImageStreamGetNextFrame(
		device->m_pVideoStreamHandle, 0, &imageFrame);

	if (FAILED(hr))
	{
		return;
	}

	INuiFrameTexture * pTexture = imageFrame.pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);
	if (LockedRect.Pitch != 0)
	{
		DWORD frameWidth, frameHeight;
		NuiImageResolutionToSize(imageFrame.eResolution, frameWidth, frameHeight);

		MiddleRgbFramePtr frame = ColorFrame::Make(
			(unsigned char*)LockedRect.pBits,
			frameWidth * frameHeight * 4,
			frameWidth,
			frameHeight);

		frame->m_frameNumber = imageFrame.dwFrameNumber;
		m_DevName2DataQueueMap[device->m_instanceName]->RenewColorFrame(frame);

		//m_pDrawColor->Draw(static_cast<BYTE *>(LockedRect.pBits), LockedRect.size);
	}
	else
	{
		OutputDebugString(L"Buffer length of received texture is bogus\r\n");
	}

	pTexture->UnlockRect(0);

	device->m_pNuiSensor->NuiImageStreamReleaseFrame(
		device->m_pVideoStreamHandle, &imageFrame);
}

void KinectDataCapturer::NuiGotSkeletonAlert(KinectDeviceInfoPtr& device)
{
	NUI_SKELETON_FRAME SkeletonFrame = { 0 };

	bool bFoundSkeleton = false;

	if (SUCCEEDED(device->m_pNuiSensor->NuiSkeletonGetNextFrame(0, &SkeletonFrame)))
	{
		for (int i = 0; i < NUI_SKELETON_COUNT; i++)
		{
			if (SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ||
				(SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY
					&& device->m_bAppTracking))
			{
				bFoundSkeleton = true;
			}
		}
	}

	// no skeletons!
	if (!bFoundSkeleton)
	{
		return;
	}

	// smooth out the skeleton data
	HRESULT hr = device->m_pNuiSensor->NuiTransformSmooth(&SkeletonFrame, NULL);
	if (FAILED(hr))
	{
		return;
	}

	// we found a skeleton, re-start the skeletal timer
	device->m_bScreenBlanked = false;
	//m_LastSkeletonFoundTime = timeGetTime();

	// draw each skeleton color according to the slot within they are found.
	//Nui_BlankSkeletonScreen(GetDlgItem(m_hWnd, IDC_SKELETALVIEW), false);

	bool bSkeletonIdsChanged = false;
	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if (device->m_SkeletonIds[i] != SkeletonFrame.SkeletonData[i].dwTrackingID)
		{
			device->m_SkeletonIds[i] = SkeletonFrame.SkeletonData[i].dwTrackingID;
			bSkeletonIdsChanged = true;
		}

		// Show skeleton only if it is tracked, and the center-shoulder joint is at least inferred.
		if (SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			SkeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			//Nui_DrawSkeleton(&SkeletonFrame.SkeletonData[i], GetDlgItem(m_hWnd, IDC_SKELETALVIEW), i);
		}
		else if (device->m_bAppTracking && SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY)
		{
			//Nui_DrawSkeletonId(&SkeletonFrame.SkeletonData[i], GetDlgItem(m_hWnd, IDC_SKELETALVIEW), i);
		}
	}

	//if (bSkeletonIdsChanged)
	//{
	//	UpdateTrackingComboBoxes();
	//}

	//Nui_DoDoubleBuffer(GetDlgItem(m_hWnd, IDC_SKELETALVIEW), m_SkeletonDC);
}

RGBQUAD KinectDataCapturer::NuiShortToQuadDepth(USHORT s)
{
	USHORT RealDepth = NuiDepthPixelToDepth(s);
	USHORT Player = NuiDepthPixelToPlayerIndex(s);

	// transform 13-bit depth information into an 8-bit intensity appropriate
	// for display (we disregard information in most significant bit)
	BYTE intensity = (BYTE)~(RealDepth >> 4);

	// tint the intensity by dividing by per-player values
	RGBQUAD color;
	color.rgbRed = intensity >> g_IntensityShiftByPlayerR[Player];
	color.rgbGreen = intensity >> g_IntensityShiftByPlayerG[Player];
	color.rgbBlue = intensity >> g_IntensityShiftByPlayerB[Player];

	return color;
}

void KinectDataCapturer::NuiUnInit(const wchar_t* uniqueName)
{
	KinectDeviceInfoPtr info = m_DevName2DevInfoMap[uniqueName];
	if (!info) return;

	// Stop the Nui processing thread
	if (NULL != info->m_hEvNuiProcessStop)
	{
		// Signal the thread
		SetEvent(info->m_hEvNuiProcessStop);

		// Wait for thread to stop
		if (NULL != info->m_hThNuiProcess)
		{
			WaitForSingleObject(info->m_hThNuiProcess, INFINITE);
			CloseHandle(info->m_hThNuiProcess);
		}
		CloseHandle(info->m_hEvNuiProcessStop);
	}

	if (info->m_pNuiSensor)
	{
		info->m_pNuiSensor->NuiShutdown();
	}
	if (info->m_hNextSkeletonEvent
		&& (info->m_hNextSkeletonEvent != INVALID_HANDLE_VALUE))
	{
		CloseHandle(info->m_hNextSkeletonEvent);
		info->m_hNextSkeletonEvent = NULL;
	}
	if (info->m_hNextDepthFrameEvent
		&& (info->m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE))
	{
		CloseHandle(info->m_hNextDepthFrameEvent);
		info->m_hNextDepthFrameEvent = NULL;
	}
	if (info->m_hNextColorFrameEvent
		&& (info->m_hNextColorFrameEvent != INVALID_HANDLE_VALUE))
	{
		CloseHandle(info->m_hNextColorFrameEvent);
		info->m_hNextColorFrameEvent = NULL;
	}

	if (info->m_pNuiSensor)
	{
		info->m_pNuiSensor->Release();
		info->m_pNuiSensor = NULL;
	}

	m_DevName2DevInfoMap[uniqueName] = NULL;
	//service capture 可能拥有此智能指针，调用此函数告知设备断开
	m_DevName2DataQueueMap[uniqueName]->StopProvideData();
	m_DevName2DataQueueMap[uniqueName] = NULL; //service capture 后面陆续会删除智能指针
}