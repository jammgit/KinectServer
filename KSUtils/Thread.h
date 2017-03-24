#pragma once

#include <mutex>
#include <thread>

class Thread
{
public:
	Thread();
	virtual ~Thread();

	virtual void Start();
	virtual void Stop();
	bool Working();

protected:
	virtual void WorkingFunc() = 0;
	volatile bool m_bWorkingSwitch;

private:
	std::mutex m_tMutex;
	std::thread *m_thread;
};
