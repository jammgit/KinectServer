#pragma once

#include <string>
#include <mutex>
#include <objbase.h>

class GuidProcessUtil
{
public:

	static std::string CreateStrGuid();

	static GUID CreateGuid();

private:
	static std::mutex m_cMutex;
};

