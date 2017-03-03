#pragma once

#include "IKSSkeletonService.h"

class KSSession;

class KSSkeletonService
	: public IKSSkeletonService
{
public:
	KSSkeletonService(KSSession *session);
	~KSSkeletonService();

	void DoFrame(const ShareFrame& frame) override;

private:
	KSSession* m_pSession;
};
