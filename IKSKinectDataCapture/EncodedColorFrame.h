#pragma once

#include "Basic264Frame.h"

class EncodedColorFrame;
typedef boost::shared_ptr<EncodedColorFrame> EncodedColorFramePtr;

class EncodedColorFrame
	: public Middle264Frame
{
};

//
//class EncodedColorFrame
//{
//public:
//	EncodedColorFrame(
//		unsigned int number,
//		unsigned int type,
//		unsigned int length,
//		unsigned char* data)
//
//	{
//		m_u32FrameNumber = number;
//		m_u32FrameType = type;
//		m_u32Length = length;
//		m_Data = data;
//	}
//	~EncodedColorFrame()
//	{
//		//delete[] m_Data;
//	}
//
//	static EncodedColorFramePtr Make(
//		unsigned int number,
//		unsigned int type,
//		unsigned int length,
//		unsigned char* data)
//	{
//		return boost::make_shared<EncodedColorFrame>(
//			number, type, length, data);
//	}
//
//public:
//	union
//	{
//		unsigned char m_FrameNumber[4];
//		unsigned int m_u32FrameNumber;
//	};
//	union
//	{
//		unsigned char m_FrameType[4];
//		unsigned int m_u32FrameType;
//	};
//	union
//	{
//		unsigned char m_Length[4];
//		unsigned int m_u32Length;
//	};
//	unsigned char* m_Data;
//};
