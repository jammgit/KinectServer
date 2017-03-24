#pragma once

#include "Basic264Frame.h"

class EncodedDepthFrame;
typedef boost::shared_ptr<EncodedDepthFrame> EncodedDepthFramePtr;

class EncodedDepthFrame
	: public Middle264Frame
{/*
public:
	using Basic264Frame<EncodedDepthFrame>::Basic264Frame;*/
};

//
//class EncodedDepthFrame
//{
//public:
//	EncodedDepthFrame(
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
//	~EncodedDepthFrame()
//	{
//		//delete[] m_Data;
//	}
//
//	static EncodedDepthFramePtr Make(
//		unsigned int number,
//		unsigned int type,
//		unsigned int length,
//		unsigned char* data)
//	{
//		return boost::make_shared<EncodedDepthFrame>(
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
