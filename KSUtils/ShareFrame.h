#pragma once

#include <boost\shared_ptr.hpp>
#include <boost\make_shared.hpp>

class FrameBuffer;
typedef boost::shared_ptr<FrameBuffer> ShareFrame;

class FrameBuffer
{
public:
	FrameBuffer(unsigned int len);
	~FrameBuffer();

	static ShareFrame Make(int len);
	static ShareFrame Make(FrameBuffer frame);
	static ShareFrame Make(
		const std::string& data,
		const unsigned char& type,
		const unsigned char& num,
		const unsigned char& ver,
		const unsigned int& seq);

public:
	union
	{
		unsigned char m_length[4];
		unsigned int m_u32length;
	};
	unsigned char m_version;
	union
	{
		unsigned char m_sequence[4];
		unsigned int m_u32Sequence;
	};
	unsigned char m_cmdType;
	unsigned char m_cmdNum;
	unsigned char *m_data;
};
