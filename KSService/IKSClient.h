#pragma once

#include "../KSKinectDataService/IKSKinectDataClient.h"
#include "../IKSKinectDataCapture/IKDCaptureClient.h"

class IKSClient
	: public IKSKinectDataClient
	, public IKDCaptureClient
{
public:
	virtual ~IKSClient() {};
};
