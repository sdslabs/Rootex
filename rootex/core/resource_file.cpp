#include "resource_file.h"

ResourceFile::ResourceFile(const Type& type, const FilePath& path)
    : m_Type(type)
    , m_Path(path.generic_string())
{
	m_LastReadTime = OS::s_FileSystemClock.now();
	m_LastChangedTime = OS::GetFileLastChangedTime(getPath().string());
}

FilePath ResourceFile::getPath() const
{
	return m_Path;
}

ResourceFile::Type ResourceFile::getType() const
{
	return m_Type;
}

const FileTimePoint& ResourceFile::getLastChangedTime()
{
	m_LastChangedTime = OS::GetFileLastChangedTime(getPath().generic_string());
	return m_LastChangedTime;
}

void ResourceFile::reimport()
{
	m_LastReadTime = OS::s_FileSystemClock.now();
	m_LastChangedTime = OS::GetFileLastChangedTime(getPath().string());
}

bool ResourceFile::save()
{
	return false;
}

bool ResourceFile::isDirty()
{
	return getLastReadTime() < getLastChangedTime();
}
