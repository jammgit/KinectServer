#pragma once

#include "../KSUtils/ShareFrame.h"

class IKSSkeletonService
{
public:
	virtual ~IKSSkeletonService() {};

	virtual void DoFrame(const ShareFrame& frame) = 0;
};
