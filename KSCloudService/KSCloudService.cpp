#include "KSCloudService.h"

KSCloudService::KSCloudService(KSSession *session)
	: m_pSession(session)
{
}

KSCloudService::~KSCloudService()
{
}

void KSCloudService::DoFrame(const ShareFrame& frame)
{
	switch (frame->m_cmdNum & 0xFF)
	{
	default:
		break;
	}
}