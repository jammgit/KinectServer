#pragma once

#include "BasicRgbFrame.h"

class ColorFrame;
typedef boost::shared_ptr<ColorFrame> ColorFramePtr;
typedef std::list<ColorFramePtr> ColorFramePtrList;

class ColorFrame
	: public MiddleRgbFrame
{
};
