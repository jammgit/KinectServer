#include "SocketAddress.h"

bool _address::operator<(const _address& addr) const
{
	if (this->addr < addr.addr
		&& this->port < addr.port)
		return true;
	return false;
}

bool _address::operator==(const _address& addr) const
{
	if (this->addr == addr.addr
		&& this->port == addr.port)
		return true;
	return true;
}

bool _addresses::operator<(const _addresses& addr) const
{
	if (this->addr[0] < addr.addr[0]
		&& this->addr[1] < addr.addr[1]
		&& this->addr[2] < addr.addr[2])
		return true;
	return false;
}

bool _addresses::operator==(const _addresses& addr) const
{
	if (this->addr[0] == addr.addr[0]
		&& this->addr[1] == addr.addr[1]
		&& this->addr[2] == addr.addr[2])
		return true;
	return false;
}

bool _addresses::Contains(const _address& addr) const
{
	if (this->addr[0] == addr
		|| this->addr[1] == addr
		|| this->addr[2] == addr)
		return true;
	return false;
}