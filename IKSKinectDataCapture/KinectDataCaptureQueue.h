#pragma once

#include "ColorFrame.h"
#include "DepthFrame.h"
#include "SkeletonFrame.h"
#include <mutex>
#include <boost\shared_ptr.hpp>

class KinectDataCaptureQueue;
typedef boost::shared_ptr<KinectDataCaptureQueue> KinectDataCaptureQueuePtr;

class KinectDataCaptureQueue
{
public:
	KinectDataCaptureQueue() :m_bStopProvideData(false) {};
	~KinectDataCaptureQueue() {};

	void StopProvideData();
	bool IsStopProvideData();

	virtual bool RenewColorFrame(const MiddleRgbFramePtr& frame);
	virtual bool GetColorFrame(MiddleRgbFramePtr& frame);

	virtual bool RenewDepthFrame(const MiddleRgbFramePtr& frame);
	virtual bool GetDepthFrame(MiddleRgbFramePtr& frame);

	virtual bool RenewSkeleFrame(const SkeletonFramePtr& frame);
	virtual bool GetSkeleFrame(SkeletonFramePtr& frame);

private:
	const size_t MAX_BUFF_SIZE = 20;
	std::mutex m_ColorMutex;
	std::mutex m_DepthMutex;
	std::mutex m_SkeletonMutex;

	MiddleRgbFramePtrList m_ColorList;
	MiddleRgbFramePtrList m_DepthList;
	SkeletonFramePtrList m_SkeletonList;

	bool m_bStopProvideData;
};
