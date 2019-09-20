#include "resource.h"

Resource::Resource(const std::string& path)
    : m_Path(path)
{
}

Resource::Resource(Resource&& oldResource)
{
	m_Path = oldResource.m_Path;
}

Resource::~Resource()
{
	m_Path = "";
}

bool Resource::isValid()
{
	return m_Path != "";
}

std::string Resource::getPath()
{
	return m_Path;
}

TextFile::TextFile(const std::string& path, std::string& contents)
    : Resource(path), m_Data(contents)
{
}

TextFile::TextFile(TextFile&& oldFile)
    : Resource(oldFile.m_Path)
{
	m_Data = oldFile.m_Data;
	m_Path = oldFile.m_Path;
}

TextFile::~TextFile()
{
	m_Data = "";
}

std::string TextFile::getContents()
{
	return m_Data;
}
