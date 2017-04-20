#pragma once

#define _X86_
#include <vector>
#include <windef.h>

class IKDCaptureClient
{
public:
	virtual ~IKDCaptureClient() {};

	virtual void DrawLine(std::vector<POINT> points) = 0;

};
