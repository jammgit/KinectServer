#pragma once

typedef struct _address
{
	unsigned long addr;
	unsigned short port;

	bool operator<(const _address& addr) const;
	bool operator==(const _address& addr) const;
}Address;

typedef struct _addresses
{
	_address addr[3];

	bool operator<(const _addresses& addr) const;
	bool operator==(const _addresses& addr) const;

	bool Contains(const _address& addr) const;
}Addresses;
