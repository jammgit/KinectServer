#pragma once

#include "../KSUtils/ShareFrame.h"

class IKSKinectDataService
{
public:
	enum
	{
		CMD_NUM_CLI_START_REQ = 1,
		CMD_NUM_CLI_END_REQ = 2,
		CMD_NUM_SVR_START_RESP = 100,
		CMD_NUM_SVR_END_REQ = 101,
	};

	typedef enum
	{
		DATA_TYPE_SKELETON = 1,
		DATA_TYPE_DEPTH = 2,
		DATA_TYPE_COLOR = 3,
	}eDataType;

	typedef enum
	{
		SUCCESS = 1,
		FAIL = -100,
	}sResultType;

public:
	virtual ~IKSKinectDataService() {};

	virtual void DoFrame(const ShareFrame& frame) = 0;

	virtual void SendEnd() = 0;
};
