#include "core/resource_data.h"

const char* ResourceData::getRawData()
{
	return m_Buffer;
}

unsigned int ResourceData::getRawDataByteSize()
{
	return m_BufferByteSize;
}

ResourceData::ResourceData(const char* data, const unsigned int byteSize)
    : m_Buffer(data)
    , m_BufferByteSize(byteSize)
{
}

ResourceData::~ResourceData()
{
	delete m_Buffer;
}
