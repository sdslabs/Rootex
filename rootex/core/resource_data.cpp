#include "core/resource_data.h"

unsigned int ResourceData::s_Count = 0;

unsigned int ResourceData::getID()
{
	return m_ID;
}

std::vector<char> ResourceData::getRawData()
{
	return m_Buffer;
}

unsigned int ResourceData::getRawDataByteSize()
{
	return m_Buffer.size();
}

ResourceData::ResourceData(std::vector<char> data)
    : m_ID(s_Count)
    , m_Buffer(data)
{
	s_Count++;
}

ResourceData::~ResourceData()
{
}
