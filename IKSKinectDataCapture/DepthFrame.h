#pragma once

#include "BasicRgbFrame.h"

class DepthFrame;
typedef boost::shared_ptr<DepthFrame> DepthFramePtr;
typedef std::list<DepthFramePtr> DepthFramePtrList;

class DepthFrame
	: public MiddleRgbFrame
{
};
//
//class DepthFrame
//{
//public:
//	unsigned char* m_rgb;
//	unsigned int m_len;
//	unsigned int m_w;
//	unsigned int m_h;
//	unsigned int m_frameNumber; //Ö¡±àºÅ
//
//	DepthFrame(
//		unsigned char* rgb,
//		unsigned int len,
//		unsigned int w,
//		unsigned int h)
//	{
//		if (!rgb || len <= 0 || w <= 0 || h <= 0)
//		{
//			m_rgb = NULL;
//			m_len = 0;
//			m_w = 0;
//			m_h = 0;
//		}
//		else
//		{
//			m_len = len;
//			m_w = w;
//			m_h = h;
//			m_rgb = new unsigned char[m_len] {0};
//			if (m_rgb)
//				memcpy(m_rgb, rgb, m_len);
//		}
//	}
//
//	~DepthFrame()
//	{
//		if (m_rgb) { delete[] m_rgb; m_rgb = NULL; }
//	}
//
//public:
//
//	static DepthFramePtr Make(
//		unsigned char* rgb,
//		unsigned int len,
//		unsigned int w,
//		unsigned int h)
//	{
//		return boost::make_shared<DepthFrame>(rgb, len, w, h);
//	}
//};
