#pragma once

#include "Basic264Frame.h"

class EncodedDepthFrame;
typedef boost::shared_ptr<EncodedDepthFrame> EncodedDepthFramePtr;

class EncodedDepthFrame
	: public Middle264Frame
{
};
