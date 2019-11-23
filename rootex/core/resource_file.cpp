#include "resource_file.h"

#include <bitset>
#include <sstream>

#include "core/resource_data_reserve.h"

ResourceFile::ResourceFile(const String& name, const String& path, const Type& type)
    : m_Name(name)
    , m_Path(path)
    , m_Type(type)
    , m_ResourceData(nullptr)
{
	// Registration in the ResourceManager is handled by ResourceLoader
}

ResourceFile::~ResourceFile()
{
}

bool ResourceFile::isValid()
{
	return m_Path != "";
}

bool ResourceFile::isOpen()
{
	return m_ResourceData == nullptr;
}

String ResourceFile::getName()
{
	return m_Name;
}

String ResourceFile::getPath()
{
	return m_Path;
}

ResourceFile::Type ResourceFile::getType()
{
	return m_Type;
}

ResourceData* ResourceFile::getData()
{
	return m_ResourceData;
}

String ResourceFile::getDataString()
{
	Vector<char>& buffer = m_ResourceData->getRawData();

	return String(buffer.begin(), buffer.end());
}

String ResourceFile::getText()
{
	return getDataString();
}

void ResourceFile::setResourceData(ResourceData* resourceData)
{
	m_ResourceData = resourceData;
}
