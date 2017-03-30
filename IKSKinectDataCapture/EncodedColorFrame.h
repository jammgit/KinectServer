#pragma once

#include "Basic264Frame.h"

class EncodedColorFrame;
typedef boost::shared_ptr<EncodedColorFrame> EncodedColorFramePtr;

class EncodedColorFrame
	: public Middle264Frame
{
};
