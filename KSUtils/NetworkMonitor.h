#pragma once

#include "Thread.h"
#include <string>
#include <boost\shared_ptr.hpp>

class INetworkClient
{
public:
	virtual void NetworkInfo(double quality, std::string ssid, std::string ip) = 0;
};
typedef boost::shared_ptr<INetworkClient> INetworkClientPtr;

class NetworkMonitor;
typedef boost::shared_ptr<NetworkMonitor> NetworkMonitorPtr;

class NetworkMonitor
	: public Thread
{
public:
	NetworkMonitor();
	~NetworkMonitor();

	void RegisterClient(INetworkClient *client);

protected:
	void WorkingFunc() override;
	void GetIp(const std::string& description, std::string& ip);

private:
	const int DURATION = 1000; //ms
	INetworkClient *m_pClient;
};

