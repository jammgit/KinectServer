#pragma once

#include "IKSCloudService.h"

class KSSession;

class KSCloudService
	: public IKSCloudService
{
public:
	KSCloudService(KSSession *session);
	~KSCloudService();

	void DoFrame(const ShareFrame& frame) override;

private:
	KSSession *m_pSession;
};
