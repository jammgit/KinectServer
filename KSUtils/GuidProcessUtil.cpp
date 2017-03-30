#include "GuidProcessUtil.h"

std::mutex GuidProcessUtil::m_cMutex;

std::string GuidProcessUtil::CreateStrGuid()
{
	GUID guid = GuidProcessUtil::CreateGuid();
	char buf[256] = { 0 };
	_snprintf_s(
		buf,
		sizeof(buf),
		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	std::string sGuid(buf);
	return sGuid;
}

GUID GuidProcessUtil::CreateGuid()
{
	std::lock_guard<std::mutex> lock(m_cMutex);
	GUID guid = { 0 };
	CoCreateGuid(&guid);
	return guid;
}

