#include "KSSession.h"
#include "../KSCloudService/KSCloudService.h"
#include "../KSSkeletonService/KSSkeletonService.h"
#include <vector>

KSSession::KSSession(socket_ptr sock)
	: m_pCloudService(NULL)
	, m_pSkeletonService(NULL)
	, AsyncTcpConnection(sock)
{
}

KSSession::~KSSession()
{
}

void KSSession::RegisterAllService()
{
	if (!m_pCloudService) m_pCloudService = new KSCloudService(this);
	if (!m_pSkeletonService) m_pSkeletonService = new KSSkeletonService(this);
}

void KSSession::SendShareFrame(ShareFrame frame)
{
	ShareData data;
	m_protocProc.TryPack(frame, data);
	if (NULL != data) this->SendShareData(data);
}

void KSSession::TryParse(const ShareData& data)
{
	std::vector<ShareFrame> frames;
	m_protocProc.TryParse(data, frames);
	for each(auto frame in frames)
	{
		switch (frame->m_cmdType & 0xFF)
		{
		case IKSSession::CMD_TYPE_LINK: {
			this->DoFrame(frame);
			break;
		}
		case IKSSession::CMD_TYPE_CLOUD: {
			if (m_pCloudService)
				m_pCloudService->DoFrame(frame);
			break;
		}
		case IKSSession::CMD_TYPE_SKELETON: {
			if (m_pSkeletonService)
				m_pSkeletonService->DoFrame(frame);
			break;
		}
		default:
			break;
		}
	}
}

void KSSession::DoFrame(const ShareFrame& frame)
{
	switch (frame->m_cmdNum & 0xFF)
	{
	default:
		break;
	}
}