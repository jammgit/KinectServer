#pragma once

#include <mutex>
#include <boost\shared_ptr.hpp>
#include <string>
#include <map>

class KSKDServiceStateMachine;
typedef boost::shared_ptr<KSKDServiceStateMachine> KSKDServiceStateMachinePtr;


class KSKDServiceStateMachine
{
public:
	typedef enum
	{
		STATE_CLI_REQ_START,
		STATE_CLI_REQ_END,
		STATE_SVR_REQ_END,
	}eState;
public:
	KSKDServiceStateMachine();
	~KSKDServiceStateMachine();

	bool IsClientCanReqStart(const std::string& dev);
	bool IsClientCanReqEnd(const std::string& dev);
	bool IsServerCanReqEnd(const std::string& dev);

private:
	std::mutex m_StateMutex;

	std::map<std::string, eState> m_DevStateMap;
};

