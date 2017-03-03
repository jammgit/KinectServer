#include "ShareFrame.h"

FrameBuffer::FrameBuffer(unsigned int len)
	: m_u32length(len)
	, m_data(NULL)
{
	if (m_u32length > 0) m_data = new unsigned char[m_u32length] {0};
}

FrameBuffer::~FrameBuffer()
{
	if (m_data) delete[] m_data;
}

ShareFrame FrameBuffer::Make(int len)
{
	return boost::make_shared<FrameBuffer>(len);
}

ShareFrame FrameBuffer::Make(FrameBuffer frame)
{
	ShareFrame sFrame = FrameBuffer::Make(frame.m_u32length);
	sFrame->m_u32length = frame.m_u32length;
	sFrame->m_u32Sequence = frame.m_u32Sequence;
	sFrame->m_version = frame.m_version;
	sFrame->m_cmdType = frame.m_cmdType;
	sFrame->m_cmdNum = frame.m_cmdNum;
	if (frame.m_u32length > 0)
		memcpy(sFrame->m_data, frame.m_data, frame.m_u32length);
	return sFrame;
}

ShareFrame FrameBuffer::Make(
	const std::string& data,
	const unsigned char& type,
	const unsigned char& num,
	const unsigned char& ver,
	const unsigned int& seq)
{
	ShareFrame sFrame = FrameBuffer::Make(data.length());
	sFrame->m_u32length = data.length();
	sFrame->m_u32Sequence = seq;
	sFrame->m_cmdType = type;
	sFrame->m_version = ver;
	sFrame->m_cmdNum = num;
	if (data.length() > 0)
		memcpy(sFrame->m_data, data.c_str(), data.length());
	return sFrame;
}