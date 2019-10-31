#include "resource_file.h"

#include <bitset>
#include <sstream>

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
	return m_ResourceData;
}

TextResource::TextResource(const String& name, const String& path, const Type& type)
    : IResourceFile(name, path, type)
{
}

String TextResource::getText()
{
	Vector<char> buffer = m_ResourceData->getRawData();
	return String(buffer.data(), buffer.size());
}

LuaScriptResource::LuaScriptResource(const String& name, const String& path)
	: TextResource(name, path, Type::LUA)
{
}

String LuaScriptResource::getSource()
{
	return getText();
}

WAVResource::WAVResource(const String& name, const String& path)
    : IResourceFile(name, path, Type::WAV)
{
}
