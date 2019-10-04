#include "resource.h"

#include "core/resource_manager.h"

Resource::Resource(const std::string& path)
    : m_Path(path)
{
	ResourceManager::registerResource(this);
}

Resource::Resource(Resource&& oldResource)
{
	ResourceManager::registerResource(this);

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

const char* Resource::getPathCStyle()
{
	return getPath().c_str();
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

std::string TextFile::getContents() const
{
	return m_Data;
}

Script::Script(const std::string& path, std::string& contents)
    : TextFile(path, contents)
    , m_Type("Lua")
{
}
