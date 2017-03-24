#pragma once

#include "../KSUtils/Singleton.h"

class IKSLogClient;

class KSLogService
	: public Singleton<KSLogService>
{
public:
	KSLogService();
	~KSLogService();

	void RegisterClient(IKSLogClient* client);

	void OutputMessage(const char* msg);
	void OutputDevice(const char*, bool);
	void OutputClient(const std::string& name, bool);

private:
	IKSLogClient *m_pConsoleClient;
};
