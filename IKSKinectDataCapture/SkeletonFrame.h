#pragma once

#include <boost\shared_ptr.hpp>
#include <list>

class SkeletonFrame;
typedef boost::shared_ptr<SkeletonFrame> SkeletonFramePtr;
typedef std::list<SkeletonFramePtr> SkeletonFramePtrList;

/*

NUI_SKELETON_FRAME
{
	DWORD dwFrameNumber;

	NUI_SKELETON_DATA
	{
		enum NUI_SKELETON_TRACKING_STATE eTrackingState;
		enum NUI_SKELETON_POSITION_TRACKING_STATE eSkeletonPositionTrackingState[20];
		Vector4 Position;
		Vector4 SkeletonPositions[20];
	}
}

*/

#pragma pack(push, 1)
typedef struct _MyVector4
{
	float x;
	float y;
	float z;
	float w;
} MyVector4;
#pragma pack(pop)

class SkeletonFrame
{
public:
	SkeletonFrame() {}
	~SkeletonFrame() {}

	static SkeletonFramePtr Make()
	{
		return boost::make_shared<SkeletonFrame>();
	}

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
	unsigned char m_TrackingState;
	unsigned char m_PositionTrackingState[20];
	union
	{
		unsigned char m_Position[4 * 4];
		MyVector4 m_fPosition;
	};
	union
	{
		unsigned char m_SkeletionPosition[20 * 4 * 4];
		MyVector4 m_fSkeletonPositions[20];
	};
};
