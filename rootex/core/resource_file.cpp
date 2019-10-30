#include "resource_file.h"

#include "core/resource_manager.h"

IResourceFile::IResourceFile(const String& name, const String& path, const Type& type)
    : m_Name(name)
    , m_Path(path)
    , m_Type(type)
    , m_ResourceData(nullptr)
{
	// Registration in the ResourceManager is handled by ResourceLoader
}

IResourceFile::~IResourceFile()
{
	m_Path = "";
}

bool IResourceFile::isValid()
{
	return m_Path != "";
}

bool IResourceFile::isOpen()
{
	return m_ResourceData == nullptr;
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

ResourceData* IResourceFile::getData()
{
	return m_ResourceData.get();
}

TextResource::TextResource(const String& name, const String& path)
    : IResourceFile(name, path, Type::TXT)
{
}

LuaScriptResource::LuaScriptResource(const String& name, const String& path)
	: IResourceFile(name, path, Type::LUA)
{
}

WAVResource::WAVResource(const String& name, const String& path)
    : IResourceFile(name, path, Type::WAV)
{
}
