#include "KinectDataCaptureQueue.h"

void KinectDataCaptureQueue::StopProvideData()
{
	m_bStopProvideData = true;
}

bool KinectDataCaptureQueue::IsStopProvideData()
{
	return m_bStopProvideData;
}

bool KinectDataCaptureQueue::RenewColorFrame(const MiddleRgbFramePtr& frame)
{
	std::lock_guard<std::mutex> lock(m_ColorMutex);
	if (m_ColorList.size() > MAX_BUFF_SIZE)
		m_ColorList.pop_front();
	m_ColorList.push_back(frame);
	return true;
}

bool KinectDataCaptureQueue::GetColorFrame(MiddleRgbFramePtr& frame)
{
	if (m_ColorList.size() > 0)
	{
		std::lock_guard<std::mutex> lock(m_ColorMutex);
		if (m_ColorList.size() > 0)
		{
			frame = m_ColorList.front();
			//m_ColorList.pop_front();
			return true;
		}
	}
	return false;
}

bool KinectDataCaptureQueue::RenewDepthFrame(const MiddleRgbFramePtr& frame)
{
	std::lock_guard<std::mutex> lock(m_DepthMutex);
	if (m_DepthList.size() > MAX_BUFF_SIZE)
		m_DepthList.pop_front();
	m_DepthList.push_back(frame);
	return true;
}

bool KinectDataCaptureQueue::GetDepthFrame(MiddleRgbFramePtr& frame)
{
	if (m_DepthList.size() > 0)
	{
		std::lock_guard<std::mutex> lock(m_DepthMutex);
		if (m_DepthList.size() > 0)
		{
			frame = m_DepthList.front();
			//m_DepthList.pop_front();
			return true;
		}
	}
	return false;
}

bool KinectDataCaptureQueue::RenewSkeleFrame(const SkeletonFramePtr& frame)
{
	std::lock_guard<std::mutex> lock(m_SkeletonMutex);
	if (m_SkeletonList.size() > MAX_BUFF_SIZE)
		m_SkeletonList.pop_front();
	m_SkeletonList.push_back(frame);
	return true;
}

bool KinectDataCaptureQueue::GetSkeleFrame(SkeletonFramePtr& frame)
{
	if (m_SkeletonList.size() > 0)
	{
		std::lock_guard<std::mutex> lock(m_SkeletonMutex);
		if (m_SkeletonList.size() > 0)
		{
			frame = m_SkeletonList.front();
			//m_SkeletonList.pop_front();
			return true;
		}
	}
	return false;
}