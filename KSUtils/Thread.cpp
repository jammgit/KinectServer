#include "Thread.h"

Thread::Thread()
	: m_thread(NULL)
	, m_bWorkingSwitch(false)
{
}

Thread::~Thread()
{
	Stop();
}

void Thread::Start()
{
	std::lock_guard<std::mutex> lock(m_tMutex);
	if (!m_thread && !m_bWorkingSwitch)
	{
		m_bWorkingSwitch = true;
		m_thread = new std::thread(std::bind(&Thread::WorkingFunc, this));
		if (!m_thread)
			m_bWorkingSwitch = false;
	}
}

void Thread::Stop()
{
	if (m_thread)
	{
		std::lock_guard<std::mutex> lock(m_tMutex);
		m_bWorkingSwitch = false;
		if (m_thread)
		{
			m_thread->join();
			delete m_thread;
			m_thread = NULL;
		}
	}
}

bool Thread::Working()
{
	return m_bWorkingSwitch;
}