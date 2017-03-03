#pragma once

#include "ShareData.h"
#include "ShareFrame.h"
#include <vector>

class ProtocProcess
{
	enum Status
	{
		WAIT_LEN_0,
		WAIT_LEN_1,
		WAIT_LEN_2,
		WAIT_LEN_3,
		WAIT_VER,
		WAIT_SEQ_0,
		WAIT_SEQ_1,
		WAIT_SEQ_2,
		WAIT_SEQ_3,
		WAIT_CMD_TYPE,
		WAIT_CMD_NUM,
		WAIT_CMD_DATA
	};
public:
	ProtocProcess();
	~ProtocProcess();

	void TryParse(const ShareData& data, std::vector<ShareFrame>& frameList);
	void TryPack(const ShareFrame& frame, ShareData& data);

private:
	int m_DataLen;
	Status m_Status;
	FrameBuffer m_Frame;
};
