#pragma once

#include "BasicRgbFrame.h"

class DepthFrame;
typedef boost::shared_ptr<DepthFrame> DepthFramePtr;
typedef std::list<DepthFramePtr> DepthFramePtrList;

class DepthFrame
	: public MiddleRgbFrame
{
};
