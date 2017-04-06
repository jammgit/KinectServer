#include "KinectDataCapturer.h"
#include "KinectDataConst.h"
#include "../KSUtils/CharsetUtils.h"
#include "../KSLogService/KSLogService.h"
#include "IKDCaptureClient.h"

#include <NuiApi.h>
#include <vector>


KinectDataCapturer::KinectDataCapturer()
	: m_pClient(NULL)
{
}

KinectDataCapturer::~KinectDataCapturer()
{
}

void KinectDataCapturer::RegisterClient(IKDCaptureClient *client)
{
	m_pClient = client;
}

void KinectDataCapturer::RegisterDevStatusCallBack()
{
	// 设备初始化同步处理
	std::lock_guard<std::mutex> lock(m_callBackMutex);
	NuiSetDeviceStatusCallback(
		&KinectDataCapturer::NuiStatusProcThunk, this);

	this->NuiInit();
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

HRESULT KinectDataCapturer::NuiInit()
{
	HRESULT  hr = S_OK;
	int idx = 0;
	while (true)
	{
		KinectDeviceInfoPtr devInfo = boost::make_shared<KinectDeviceInfo>();
		hr = NuiCreateSensorByIndex(idx, &devInfo->m_pNuiSensor);
		if (FAILED(hr))
			return hr;

		devInfo->m_instanceName = devInfo->m_pNuiSensor->NuiDeviceConnectionId();
		hr = this->NuiInit(devInfo);
		if (FAILED(hr))
			goto next;

		{
			std::wstring w(devInfo->m_instanceName);
			std::string a;
			CharsetUtils::UnicodeStringToANSIString(w, a);
			KSLogService::GetInstance()->OutputDevice(a, true);
		}
	next:
		++idx;
	}
	return hr;
}

void CALLBACK KinectDataCapturer::NuiStatusProc(
	HRESULT hrStatus,
	const wchar_t* instanceName,
	const wchar_t* uniqueDeviceName)
{
	HRESULT hr;
	std::lock_guard<std::mutex> lock(m_callBackMutex);
	if (SUCCEEDED(hrStatus))
	{//连上
		if (S_OK == hrStatus)
		{
			KinectDeviceInfoPtr devInfo = boost::make_shared<KinectDeviceInfo>();
			devInfo->m_instanceName = instanceName;
			devInfo->m_deviceName = uniqueDeviceName;


			if (devInfo->m_instanceName.empty())
				goto initerror;

			hr = NuiCreateSensorById(devInfo->m_instanceName.c_str(), &devInfo->m_pNuiSensor);
			if (FAILED(hr))
				goto initerror;

			devInfo->m_instanceId = devInfo->m_pNuiSensor->NuiDeviceConnectionId();

			hr = this->NuiInit(devInfo);
			if (FAILED(hr))
				goto initerror;

			{
				std::wstring w(instanceName);
				std::string a;
				CharsetUtils::UnicodeStringToANSIString(w, a);
				KSLogService::GetInstance()->OutputDevice(a, true);
			}
			return;

		initerror:
			return;
		}
	}
	else
	{//断开连接
		if (instanceName == NULL) return;

		NuiUnInit(instanceName);

		std::wstring w(instanceName);
		std::string a;
		CharsetUtils::UnicodeStringToANSIString(w, a);
		KSLogService::GetInstance()->OutputDevice(a, false);
	}
}


HRESULT KinectDataCapturer::NuiInit(KinectDeviceInfoPtr& device)
{
	HRESULT hr;

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
		hr = device->m_pNuiSensor->NuiSkeletonTrackingEnable(device->m_hNextSkeletonEvent,
			/*NUI_SKELETON_TRACKING_FLAG_TITLE_SETS_TRACKED_SKELETONS*/0);
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

	//创建map
	m_DevName2DevInfoMap[device->m_instanceName] = device; //device 是引用形参
	m_DevName2DevInfoMap[device->m_instanceName]->m_pKinectDataCapturer = this;

	m_DevName2DataQueueMap[device->m_instanceName] =
		boost::make_shared<KinectDataCaptureQueue>();

	std::lock_guard<std::mutex> lock(m_ListMutex);
	m_DevNameList.push_back(device->m_instanceName);

	// Start the Nui processing thread
	device->m_hEvNuiProcessStop = CreateEvent(NULL, FALSE, FALSE, NULL);
	device->m_hThNuiProcess = CreateThread(
		NULL, 0, NuiProcessThread, &m_DevName2DevInfoMap[device->m_instanceName]/*this*/, 0, NULL);

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
			if (SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
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

	SkeletonFramePtr sFrame = SkeletonFrame::Make(
		SkeletonFrame.dwFrameNumber, 320, 240);

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
			USHORT depth;
			POINT point[20] = { 0 };
			for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)
				NuiTransformSkeletonToDepthImage(SkeletonFrame.SkeletonData[i].SkeletonPositions[j], &point[j].x, &point[j].y, &depth);
			
			if ((point[0].y >= point[1].y)
				&& (point[1].y >= point[2].y)
				&& (point[2].y >= point[3].y))
			{
			}
			else
			{
				printf("Not sequence\n");
			}

			NuiDrawSkeletonSegment(sFrame, &SkeletonFrame.SkeletonData[i], point,4, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_HEAD);
			NuiDrawSkeletonSegment(sFrame, &SkeletonFrame.SkeletonData[i], point,5, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);
			NuiDrawSkeletonSegment(sFrame, &SkeletonFrame.SkeletonData[i], point,5, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
			NuiDrawSkeletonSegment(sFrame, &SkeletonFrame.SkeletonData[i], point,5, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
			NuiDrawSkeletonSegment(sFrame, &SkeletonFrame.SkeletonData[i], point,5, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);
		}
	}

	if (m_pClient && 
		(sFrame->m_SkelePointCount == 24
			|| sFrame->m_SkelePointCount == 48))
	{
		std::vector<POINT> pvec;
		SkelePoint* ps = sFrame->m_SkelePoints;
		for (int i = 0; i < sFrame->m_SkelePointCount; ++i)
		{
			POINT p;
			p.x = (ps+i)->x;
			p.y = (ps+i)->y;
			pvec.push_back(p);
		}
		if (m_pClient)
			m_pClient->DrawLine(pvec);
	}
	
	m_DevName2DataQueueMap[device->m_instanceName]->RenewSkeleFrame(sFrame);

}


void KinectDataCapturer::NuiDrawSkeletonSegment(
	SkeletonFramePtr &frame, 
	NUI_SKELETON_DATA * pSkel,
	POINT points[20], 
	int numJoints, ...)
{
	va_list vl;
	va_start(vl, numJoints);

	POINT segmentPositions[NUI_SKELETON_POSITION_COUNT];
	int segmentPositionsCount = 0;

	DWORD polylinePointCounts[NUI_SKELETON_POSITION_COUNT];
	int numPolylines = 0;
	int currentPointCount = 0;

	// Note the loop condition: We intentionally run one iteration beyond the
	// last element in the joint list, so we can properly end the final polyline.
	for (int iJoint = 0; iJoint <= numJoints; iJoint++)
	{
		if (iJoint < numJoints)
		{
			NUI_SKELETON_POSITION_INDEX jointIndex = va_arg(vl, NUI_SKELETON_POSITION_INDEX);

			if (pSkel->eSkeletonPositionTrackingState[jointIndex] != NUI_SKELETON_POSITION_NOT_TRACKED)
			{
				// This joint is tracked: add it to the array of segment positions.        
				segmentPositions[segmentPositionsCount] = points[jointIndex];
				segmentPositionsCount++;
				currentPointCount++;

				// Fully processed the current joint; move on to the next one
				continue;
			}
			else
			{
				//printf("Not tracked one joint\n");
			}
		}
		// If we fall through to here, we're either beyond the last joint, or
		// the current joint is not tracked: end the current polyline here.
		if (currentPointCount > 1)
		{
			// Current polyline already has at least two points: save the count.
			polylinePointCounts[numPolylines++] = currentPointCount;
		}
		else if (currentPointCount == 1)
		{
			// Current polyline has only one point: ignore it.
			segmentPositionsCount--;
		}
		currentPointCount = 0;
	}


	if (numPolylines > 0)
	{
		frame->m_PolyLinesArr[frame->m_PolyLinesArrCount++] = segmentPositionsCount;
		for (int i = 0; i < segmentPositionsCount; ++i)
		{
			frame->m_SkelePoints[frame->m_SkelePointCount].x = (unsigned short)segmentPositions[i].x;
			frame->m_SkelePoints[frame->m_SkelePointCount].y = (unsigned short)segmentPositions[i].y;
			
			frame->m_SkelePointCount++;
		}
	}
	va_end(vl);
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

	// 删除信息
	auto mapiter = m_DevName2DevInfoMap.find(uniqueName);
	if (mapiter == m_DevName2DevInfoMap.end())
		return;
	m_DevName2DevInfoMap.erase(mapiter);

	auto datamapiter = m_DevName2DataQueueMap.find(uniqueName);
	if (datamapiter == m_DevName2DataQueueMap.end())
		return;
	datamapiter->second->StopProvideData();
	m_DevName2DataQueueMap.erase(datamapiter);

	std::lock_guard<std::mutex> lock(m_ListMutex);
	auto iter = m_DevNameList.begin();
	for each(auto elem in m_DevNameList)
	{
		if (wcsncmp(elem.c_str(), uniqueName, elem.length()) == 0)
		{
			m_DevNameList.erase(iter);
			break;
		}
		++iter;
	}
}