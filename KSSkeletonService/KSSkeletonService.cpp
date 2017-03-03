#include "KSSkeletonService.h"

KSSkeletonService::KSSkeletonService(KSSession *session)
	: m_pSession(session)
{
}

KSSkeletonService::~KSSkeletonService()
{
}

void KSSkeletonService::DoFrame(const ShareFrame& frame)
{
	switch (frame->m_cmdNum & 0xFF)
	{
	default:
		break;
	}
}