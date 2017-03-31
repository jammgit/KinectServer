#include "ProtocProcess.h"

ProtocProcess::ProtocProcess()
	: m_Status(WAIT_CMD_TYPE)
	, m_Frame(0)
{
}

ProtocProcess::~ProtocProcess()
{
}

void ProtocProcess::TryParse(const ShareData& data, std::vector<ShareFrame>& frameList)
{
	for (unsigned int i = 0; i < data->Used();)
	{
		switch (m_Status)
		{
		case ProtocProcess::WAIT_CMD_TYPE:
			m_Frame.m_cmdType = data->Data()[i++];
			m_Status = WAIT_CMD_NUM;
			break;
		case ProtocProcess::WAIT_CMD_NUM:
			m_Frame.m_cmdNum = data->Data()[i++];
			m_Status = WAIT_SEQ_0;
			break;
		case ProtocProcess::WAIT_SEQ_0:
			m_Frame.m_sequence[3] = data->Data()[i++];
			m_Status = WAIT_SEQ_1;
			break;
		case ProtocProcess::WAIT_SEQ_1:
			m_Frame.m_sequence[2] = data->Data()[i++];
			m_Status = WAIT_SEQ_2;
			break;
		case ProtocProcess::WAIT_SEQ_2:
			m_Frame.m_sequence[1] = data->Data()[i++];
			m_Status = WAIT_SEQ_3;
			break;
		case ProtocProcess::WAIT_SEQ_3:
			m_Frame.m_sequence[0] = data->Data()[i++];
			m_Status = WAIT_VER;
			break;
		case ProtocProcess::WAIT_VER:
			m_Frame.m_version = data->Data()[i++];
			m_Status = WAIT_LEN_0;
			break;
		case ProtocProcess::WAIT_LEN_0:
			m_Frame.m_length[3] = data->Data()[i++];
			m_Status = WAIT_LEN_1;
			break;
		case ProtocProcess::WAIT_LEN_1:
			m_Frame.m_length[2] = data->Data()[i++];
			m_Status = WAIT_LEN_2;
			break;
		case ProtocProcess::WAIT_LEN_2:
			m_Frame.m_length[1] = data->Data()[i++];
			m_Status = WAIT_LEN_3;
			break;
		case ProtocProcess::WAIT_LEN_3:
			m_Frame.m_length[0] = data->Data()[i++];
			if (m_Frame.m_data)
			{
				delete[] m_Frame.m_data;
				m_Frame.m_data = NULL;
			}

			if ((m_Frame.m_u32length > FrameBuffer::MIN_LEN)
				&& (m_Frame.m_u32length < FrameBuffer::MAX_LEN)) //限定包体长度
			{
				m_DataLen = 0;
				m_Status = WAIT_CMD_DATA;
				printf("包体长度[%d]bytes\n", m_Frame.m_u32length);
				m_Frame.m_data = new unsigned char[m_Frame.m_u32length]{ 0 };
			}
			else
			{// 帧信息错误
				m_Status = WAIT_CMD_TYPE;
				m_Frame.m_u32length = 0;
				frameList.push_back(FrameBuffer::Make(m_Frame));
			}
			break;
		case ProtocProcess::WAIT_CMD_DATA: {
			int leftLength = data->Used() - i;
			int needLength = m_Frame.m_u32length - m_DataLen;
			int copyLength = leftLength > needLength ? needLength : leftLength;
			memcpy(m_Frame.m_data + m_DataLen, data->Data() + i, copyLength);
			i += copyLength;
			m_DataLen += copyLength;
			if (m_Frame.m_u32length == m_DataLen)
			{
				m_Status = WAIT_CMD_TYPE;
				m_DataLen = 0;
				frameList.push_back(FrameBuffer::Make(m_Frame));
			}
			else
			{
				m_Status = WAIT_CMD_DATA;
			}
			break;
		}
		default:
			m_Status = WAIT_CMD_TYPE;
			break;
		}
	}
}

void ProtocProcess::TryPack(const ShareFrame& frame, ShareData& data)
{
	data = DataBuffer::Make(frame->m_u32length + 11);
	data->Data()[0] = frame->m_cmdType;
	data->Data()[1] = frame->m_cmdNum;
	data->Data()[2] = frame->m_sequence[3];
	data->Data()[3] = frame->m_sequence[2];
	data->Data()[4] = frame->m_sequence[1];
	data->Data()[5] = frame->m_sequence[0];
	data->Data()[6] = frame->m_version;
	data->Data()[7] = frame->m_length[3];
	data->Data()[8] = frame->m_length[2];
	data->Data()[9] = frame->m_length[1];
	data->Data()[10] = frame->m_length[0];
	if (frame->m_u32length > 0) memcpy(data->Data() + 11, frame->m_data, frame->m_u32length);
}