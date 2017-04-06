#pragma once

#include <boost\shared_ptr.hpp>
#include <list>

class SkeletonFrame;
typedef boost::shared_ptr<SkeletonFrame> SkeletonFramePtr;
typedef std::list<SkeletonFramePtr> SkeletonFramePtrList;

#pragma pack(push, 1)
typedef struct _SkelePoint
{
	union 
	{
		unsigned char cx[2];
		short x;
	};
	union
	{
		unsigned char cy[2];
		short y;
	};
	
}SkelePoint;
#pragma pack(pop)

class SkeletonFrame
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

	// m_u32Length == 以下包体长度
	union
	{
		unsigned char m_Width[2];
		unsigned short m_u16Width;
	};
	union
	{
		unsigned char m_Height[2];
		unsigned short m_u16Height;
	};
	unsigned char m_PolyLines;
	unsigned char *m_PolyLinesArr;
	unsigned short m_PolyLinesArrCount;
	SkelePoint *m_SkelePoints;
	unsigned short m_SkelePointCount;

public:
	SkeletonFrame(
		unsigned int number, //帧号
		int w, int h  //宽高
		) 
		: m_PolyLinesArr(NULL)
		, m_SkelePoints(NULL)
	{
		m_u32FrameNumber = number;
		
		m_u16Width = w;
		m_u16Height = h;

		if (w != 0 && h != 0)
		{
			m_PolyLinesArr = new unsigned char[10]; //最多10段
			m_SkelePoints = new SkelePoint[48]; //最多48个点
		}
		m_PolyLinesArrCount = 0;
		m_SkelePointCount = 0;
	}
	~SkeletonFrame()
	{
		if (m_PolyLinesArr) { delete[] m_PolyLinesArr; m_PolyLinesArr = NULL; }
		if (m_SkelePoints) { delete[] m_SkelePoints; m_SkelePoints = NULL; }
	}
	static SkeletonFramePtr Make(
		unsigned int number, //帧号
		int w, 
		int h  //宽高
		)
	{
		return boost::make_shared<SkeletonFrame>(number, w, h);
	}

};
