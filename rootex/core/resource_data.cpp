#include "core/resource_data.h"

const char* IResourceData::getRawData()
{
	return m_Buffer;
}

unsigned int IResourceData::getRawDataByteSize()
{
	return m_BufferByteSize;
}

IResourceData::IResourceData(const char* data, const unsigned int byteSize)
    : m_Buffer(data)
    , m_BufferByteSize(byteSize)
{
}

TextFileData::TextFileData(String contents)
    : IResourceData(contents.c_str(), (unsigned int)contents.length() * sizeof(char))
    , m_Text(contents)
{
}

String TextFileData::getData()
{
	return m_Text;
}

unsigned int TextFileData::getSize()
{
	return m_Text.length();
}

LuaScriptData::LuaScriptData(String source)
    : IResourceData(source.c_str(), (unsigned int)source.length() * sizeof(char))
    , m_Source(source)
{
}

String LuaScriptData::getData()
{
	return m_Source;
}

unsigned int LuaScriptData::getSize()
{
	return m_Source.length();
}

String LuaScriptData::getSource()
{
	return getData();
}
