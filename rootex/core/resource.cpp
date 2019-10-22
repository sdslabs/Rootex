#include "resource.h"

#include "core/resource_manager.h"

IResourceFile::IResourceFile(const String& name, const String& path, const Type& type)
    : m_Name(name)
    , m_Path(path)
    , m_Type(type)
{
	ResourceManager::registerResourceDataResourceFile(this);
}

IResourceFile::~IResourceFile()
{
	m_Path = "";
}

bool IResourceFile::isValid()
{
	return m_Path != "";
}

String IResourceFile::getName()
{
	return m_Name;
}

String IResourceFile::getPath()
{
	return m_Path;
}

IResourceFile::Type IResourceFile::getType()
{
	return m_Type;
}

LuaScriptResource::LuaScriptResource(const String& name, const String& path, const Type& type)
    : IResourceFile(name, path, Type::LUA)
{
}

WAVResource::WAVResource(const String& name, const String& path, const Type& type)
    : IResourceFile(name, path, Type::WAV)
{
}
