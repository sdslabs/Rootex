#include "resource.h"

#include "core/resource_manager.h"

IResource::IResource(const String& path)
    : m_Path(path)
{
	ResourceManager::registerResource(this);
}

IResource::IResource(IResource&& oldResource)
{
	ResourceManager::registerResource(this);

	m_Path = oldResource.m_Path;
}

IResource::~IResource()
{
	m_Path = "";
}

bool IResource::isValid()
{
	return m_Path != "";
}

String IResource::getPath()
{
	return m_Path;
}

const char* IResource::getPathCStyle()
{
	return getPath().c_str();
}

TextFile::TextFile(const String& path, String& contents)
    : IResource(path), m_Data(contents)
{
}

TextFile::TextFile(TextFile&& oldFile)
    : IResource(oldFile.m_Path)
{
	m_Data = oldFile.m_Data;
	m_Path = oldFile.m_Path;
}

TextFile::~TextFile()
{
	m_Data = "";
}

String TextFile::getContents() const
{
	return m_Data;
}

LuaScript::LuaScript(const String& path, String& contents)
    : TextFile(path, contents)
    , m_Type("Lua")
{
}
