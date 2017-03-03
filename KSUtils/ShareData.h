#pragma once

#include <boost\shared_ptr.hpp>

class DataBuffer;
typedef boost::shared_ptr<DataBuffer> ShareData;

class DataBuffer
{
public:
	DataBuffer(unsigned int len);
	~DataBuffer();

	static ShareData Make(unsigned int len);

	unsigned char *Data() const;
	unsigned int Length() const;
	void SetUsed(unsigned int len);
	unsigned int Used() const;

private:
	unsigned char *m_data;
	unsigned int m_len;
	unsigned int m_used;
};
