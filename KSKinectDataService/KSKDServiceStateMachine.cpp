#include "KSKDServiceStateMachine.h"



KSKDServiceStateMachine::KSKDServiceStateMachine()
{
}

KSKDServiceStateMachine::~KSKDServiceStateMachine()
{
}

bool KSKDServiceStateMachine::IsClientCanReqStart(const std::string& dev)
{
	std::lock_guard<std::mutex> lock(m_StateMutex);
	auto iter = m_DevStateMap.find(dev);
	if (iter == m_DevStateMap.end()
		|| iter->second == STATE_SVR_REQ_END
		|| iter->second == STATE_CLI_REQ_END)
	{
		m_DevStateMap[dev] = STATE_CLI_REQ_START;
		return true;
	} 
	return false;
}

bool KSKDServiceStateMachine::IsClientCanReqEnd(const std::string& dev)
{
	std::lock_guard<std::mutex> lock(m_StateMutex);
	auto iter = m_DevStateMap.find(dev);
	if (iter != m_DevStateMap.end()
		&& iter->second == STATE_CLI_REQ_START)
	{
		iter->second = STATE_CLI_REQ_END;
		return true;
	}
	return false;
}

bool KSKDServiceStateMachine::IsServerCanReqEnd(const std::string& dev)
{
	std::lock_guard<std::mutex> lock(m_StateMutex);
	auto iter = m_DevStateMap.find(dev);
	if (iter != m_DevStateMap.end()
		&& iter->second == STATE_CLI_REQ_START)
	{
		iter->second = STATE_SVR_REQ_END;
		return true;
	}
	return false;
}
