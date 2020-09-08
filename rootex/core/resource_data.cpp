#include "core/resource_data.h"

unsigned int ResourceData::s_Count = 0;

unsigned int ResourceData::getID()
{
	return m_ID;
}

FilePath ResourceData::getPath()
{
	return m_Path;
}

FileBuffer* ResourceData::getRawData()
{
	return &m_FileBuffer;
}

unsigned int ResourceData::getRawDataByteSize()
{
	return m_FileBuffer.size();
}

void ResourceData::setPath(String path)
{
	m_Path = path;
}

ResourceData& ResourceData::operator>>(char& fillIn)
{
	if (m_StreamStart != m_StreamEnd)
	{
		m_StreamStart++;
	}
	fillIn = *m_StreamStart;

	return *this;
}

ResourceData::operator bool() const
{
	return m_StreamStart != m_StreamEnd;
}

void ResourceData::startStream()
{
	m_StreamStart = &m_FileBuffer.front();
	m_StreamEnd = &m_FileBuffer.back();
}

bool ResourceData::isEndOfFile()
{
	return m_StreamStart == m_StreamEnd;
}

void ResourceData::resetStream()
{
	m_StreamStart = &m_FileBuffer.front();
}

ResourceData::ResourceData(FilePath path, FileBuffer& data)
    : m_ID(s_Count)
    , m_FileBuffer(data)
    , m_Path(path.generic_string())
{
	s_Count++;
}
