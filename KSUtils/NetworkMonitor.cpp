#include "NetworkMonitor.h"

#include <WinSock2.h>
#include <Wlanapi.h>
#include <Iphlpapi.h>

#include "CharsetUtils.h"


NetworkMonitor::NetworkMonitor()
	: m_pClient(NULL)
{
}


NetworkMonitor::~NetworkMonitor()
{
}

void NetworkMonitor::RegisterClient(INetworkClient *client)
{
	m_pClient = client;
}

void NetworkMonitor::WorkingFunc()
{
	WSAData data;
	if (WSAStartup(MAKEWORD(1, 1), &data) != 0)
	{
		return;
	}

	DWORD dwError = ERROR_SUCCESS;
	DWORD dwActualVresion;
	HANDLE wlanHandle;

	dwError = WlanOpenHandle(1, NULL, &dwActualVresion, &wlanHandle);
	if (dwError != ERROR_SUCCESS)
	{
		return;
	}

	PWLAN_INTERFACE_INFO_LIST pInterfaceList = NULL;
	dwError = WlanEnumInterfaces(wlanHandle, NULL, &pInterfaceList);
	if (dwError != ERROR_SUCCESS)
	{
		return;
	}

	if (pInterfaceList->dwNumberOfItems == 0)
	{
		return;
	}

	bool findConnectedSsid = false;
	PWLAN_AVAILABLE_NETWORK_LIST pAvailableNetworkList = NULL;


	while (m_bWorkingSwitch)
	{
		findConnectedSsid = false;
		for (DWORD cur = 0; cur < pInterfaceList->dwNumberOfItems; ++cur)
		{
			GUID &guid = pInterfaceList->InterfaceInfo[cur].InterfaceGuid;

			std::wstring wdescri(pInterfaceList->InterfaceInfo[cur].strInterfaceDescription);
			std::string descri;
			CharsetUtils::UnicodeStringToANSIString(wdescri, descri);

			dwError = WlanGetAvailableNetworkList(wlanHandle, &guid, 2, NULL, &pAvailableNetworkList);

			for (DWORD idx = 0; idx < pAvailableNetworkList->dwNumberOfItems; ++idx)
			{
				if (pAvailableNetworkList->Network[idx].dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
				{
					std::string ssid((char *)(pAvailableNetworkList->Network[idx].dot11Ssid.ucSSID));
					double quality = 1.0 * pAvailableNetworkList->Network[idx].wlanSignalQuality / 100;
					std::string ip;
					
					//this->GetIp(descri, ip);

					if (m_pClient)
						m_pClient->NetworkInfo(quality, ssid, ip);

					findConnectedSsid = true;
					break;
				}
			}
		}
		if (!findConnectedSsid)
		{
			m_pClient->NetworkInfo(0, "No signal", "0");
		}

		Sleep(DURATION);
	}

	WlanCloseHandle(wlanHandle, NULL);
}

void NetworkMonitor::GetIp(const std::string& description, std::string& ip)
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	PIP_ADAPTER_INFO curAdapter = pIpAdapterInfo;

	while (curAdapter)
	{
		if (strcmp(description.c_str(), curAdapter->Description) == 0)
		{
			std::string p(curAdapter->IpAddressList.IpAddress.String);
			ip = p;
			break;
		}

		curAdapter = curAdapter->Next;
	}

	if (pIpAdapterInfo)
		delete pIpAdapterInfo;
}
