#include "resource_file.h"

ResourceFile::ResourceFile(const Type& type, const FilePath& path)
    : m_Type(type)
    , m_Path(path.generic_string())
{
	m_LastReadTime = OS::s_FileSystemClock.now();
	m_LastChangedTime = OS::GetFileLastChangedTime(m_Path.generic_string());
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
	m_LastChangedTime = OS::GetFileLastChangedTime(m_Path.generic_string());
	return m_LastChangedTime;
}

void ResourceFile::reimport()
{
	m_LastReadTime = OS::s_FileSystemClock.now();
	m_LastChangedTime = OS::GetFileLastChangedTime(m_Path.generic_string());
}

bool ResourceFile::save()
{
	return false;
}

void ResourceFile::draw()
{
}

bool ResourceFile::isDirty()
{
	return getLastReadTime() < getLastChangedTime();
}

void to_json(JSON::json& j, const ResourceFile::Type& t)
{
	j = (int)t;
}

void from_json(const JSON::json& j, ResourceFile::Type& t)
{
	t = (ResourceFile::Type)(int)j;
}
