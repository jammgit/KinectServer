#pragma once


class SysConfig
{
public:
	SysConfig();
	~SysConfig();

	static void InitConfig();

private:
	static void ValidatePort();

};

