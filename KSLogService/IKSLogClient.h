#pragma once

#include <string>

class IKSLogClient
{
public:
	virtual ~IKSLogClient() {};

	virtual void OutputString(const char* msg) = 0;
	virtual void OutputString(const std::string& msg) = 0;
	virtual void OutputDevice(const std::string& dev, bool connect) = 0;
	virtual void OutputClient(const std::string&, bool connect) = 0;
};
