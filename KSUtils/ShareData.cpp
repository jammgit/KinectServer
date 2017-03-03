#include "ShareData.h"
#include <boost\make_shared.hpp>

DataBuffer::DataBuffer(unsigned int len)
	: m_data(NULL)
	, m_len(len)
	, m_used(0)
{
	if (len > 0) m_data = new unsigned char[len] {0};
}

DataBuffer::~DataBuffer()
{
	if (m_data) { delete[] m_data; m_data = NULL; }
}

ShareData DataBuffer::Make(unsigned int len)
{
	return boost::make_shared<DataBuffer>(len);
}

unsigned char *DataBuffer::Data() const
{
	return m_data;
}

unsigned int DataBuffer::Length() const
{
	return m_len;
}

void DataBuffer::SetUsed(unsigned int len)
{
	m_used = len;
}

unsigned int DataBuffer::Used() const
{
	return m_used;
}