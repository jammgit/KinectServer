#include "KSLogService.h"
#include "IKSLogClient.h"

KSLogService::KSLogService()
	: m_pConsoleClient(NULL)
{
}

KSLogService::~KSLogService()
{
}

void KSLogService::RegisterClient(IKSLogClient* client)
{
	m_pConsoleClient = client;
}

void KSLogService::OutputMessage(const char* msg)
{
	if (m_pConsoleClient)
		m_pConsoleClient->OutputString(msg);
}

void KSLogService::OutputDevice(const char*dev, bool connect)
{
	if (m_pConsoleClient)
		m_pConsoleClient->OutputDevice(dev, connect);
}

void KSLogService::OutputClient(const std::string& name, bool connect)
{
	if (m_pConsoleClient)
		m_pConsoleClient->OutputClient(name, connect);
}