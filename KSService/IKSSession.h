#pragma once

#include "../KSUtils/ShareFrame.h"

class IKSSession
{
public:
	enum
	{
		CMD_TYPE_LINK,
		CMD_TYPE_CLOUD,
		CMD_TYPE_SKELETON,
	};

	enum
	{
	};

public:
	virtual ~IKSSession() {};

	virtual void RegisterAllService() = 0;
	virtual void DoFrame(const ShareFrame& frame) = 0;
};
