#pragma once

#include "../KSUtils/ShareFrame.h"

class IKSCloudService
{
public:
	virtual ~IKSCloudService() {};

	virtual void DoFrame(const ShareFrame& frame) = 0;
};