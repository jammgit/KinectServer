#pragma once

#include <mutex>

template <class T>
class Singleton
{
protected:
	Singleton() {};
	virtual ~Singleton() {};

public:
	static T* GetInstance()
	{
		if (!m_self)
		{
			std::lock_guard<std::mutex> lock(m_selfMutex);
			if (!m_self)
			{
				m_self = new T();
			}
		}

		return m_self;
	};

	void ReleaseInstance()
	{
		if (m_self)
		{
			std::lock_guard<std::mutex> lock(m_selfMutex);
			if (m_self)
			{
				delete m_self;
				m_self = NULL;
			}
		}
	};

private:
	static T* m_self;
	static std::mutex m_selfMutex;
};

template <class T>
T* Singleton<T>::m_self = NULL;

template <class T>
std::mutex Singleton<T>::m_selfMutex;
