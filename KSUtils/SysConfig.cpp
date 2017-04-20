#include "SysConfig.h"
#include "PortsMacro.h"


#include <WinSock2.h>

SysConfig::SysConfig()
{
}


SysConfig::~SysConfig()
{
}

void SysConfig::InitConfig()
{
	SysConfig::ValidatePort();
}

void SysConfig::ValidatePort()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
		return ;


	unsigned short ports[4] = { PORT_KSSERVICE, PORT_COLORDATA, PORT_DEPTHDATA, PORT_SKELETONDATA };
	unsigned int pSize = 4;

	for (int idx = 0; idx < 4; ++idx)
	{
		if (idx > 0) ports[idx] = ports[idx - 1] + 1;

		SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if (slisten == INVALID_SOCKET)
			return;

		//°ó¶¨IPºÍ¶Ë¿Ú
		sockaddr_in sin;
		do
		{
			sin.sin_family = AF_INET;
			sin.sin_port = htons(ports[idx]++);
			sin.sin_addr.S_un.S_addr = INADDR_ANY;

		} while (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR);

		closesocket(slisten);

		--ports[idx];
	}

	PORT_KSSERVICE = ports[0];
	PORT_COLORDATA = ports[1];
	PORT_DEPTHDATA = ports[2];
	PORT_SKELETONDATA = ports[3];

}

