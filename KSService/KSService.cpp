#include "KSService.h"

KSService::KSService()
	: AsyncTcpServer(7890)
{
}

KSService::~KSService()
{
}

void KSService::WorkingFunc()
{
	AsyncTcpServer::Start();
}

void KSService::Stop()
{
	AsyncTcpServer::Stop();

	Thread::Stop();
}

void KSService::CreateConnection(socket_ptr sock)
{
	KSSessionPtr session = boost::make_shared<KSSession>(sock);
	ShareFrame frame;
	if (m_sessionList.size() < 16)
	{
		session->DoRead();
		m_sessionList.push_back(session);
		return;
	}
	else
	{// 回复一个失败然后断开socket
	}
}