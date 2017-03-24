#pragma once

#include <boost\shared_ptr.hpp>
#include <boost\make_shared.hpp>

template <class T>
class Basic264Frame
{
public:
	union
	{
		unsigned char m_FrameNumber[4];
		unsigned int m_u32FrameNumber;
	};
	union
	{
		unsigned char m_FrameType[4];
		unsigned int m_u32FrameType;
	};
	union
	{
		unsigned char m_Length[4];
		unsigned int m_u32Length;
	};
	unsigned char* m_Data;

public:
	Basic264Frame(
		unsigned int number,
		unsigned int type,
		unsigned int length,
		unsigned char* data)

	{
		m_u32FrameNumber = number;
		m_u32FrameType = type;
		m_u32Length = length;
		m_Data = data;
	}
	virtual ~Basic264Frame()
	{
		if (m_Data) { delete[] m_Data; m_Data = NULL; }
	}

	static boost::shared_ptr<T> Make(
		unsigned int number,
		unsigned int type,
		unsigned int length,
		unsigned char* data)
	{
		return boost::make_shared<T>(
			number, type, length, data);
	}
};

class Middle264Frame;
typedef boost::shared_ptr<Middle264Frame> Middle264FramePtr;

class Middle264Frame
	: public Basic264Frame<Middle264Frame>
{
public:
	using Basic264Frame<Middle264Frame>::Basic264Frame;
};
