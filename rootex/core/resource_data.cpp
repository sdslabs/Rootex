#include "core/resource_data.h"

unsigned int ResourceData::s_Count = 0;

unsigned int ResourceData::getID()
{
	return m_ID;
}

String ResourceData::getPath()
{
	return m_Path;
}

Vector<char>& ResourceData::getRawData()
{
	return m_Buffer;
}

unsigned int ResourceData::getRawDataByteSize()
{
	return m_Buffer.size();
}

void ResourceData::setPath(String path)
{
	m_Path = path;
}

ResourceData::ResourceData(std::vector<char> data)
    : m_ID(s_Count)
    , m_Buffer(data)
{
	s_Count++;
}
