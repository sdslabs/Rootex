#include "resource_file.h"

#include <bitset>
#include <sstream>

#include "audio/audio_system.h"
#include "core/resource_data_reserve.h"

ResourceFile::ResourceFile(const Type& type, ResourceData* resData)
    : m_Type(type)
    , m_ResourceData(resData)
{
	PANIC(resData == nullptr, "Null resource found. Resource of this type has not been loaded correctly: " + std::to_string((int)type));
}

ResourceFile::~ResourceFile()
{
}

bool ResourceFile::isValid()
{
	return m_ResourceData != nullptr;
}

bool ResourceFile::isOpen()
{
	return m_ResourceData == nullptr;
}

FilePath ResourceFile::getPath()
{
	return m_ResourceData->getPath();
}

ResourceFile::Type ResourceFile::getType()
{
	return m_Type;
}

ResourceData* ResourceFile::getData()
{
	return m_ResourceData;
}

AudioResourceFile::AudioResourceFile(ResourceData* resData)
    : ResourceFile(Type::WAV, resData)
    , m_AudioDataSize(0)
    , m_BitDepth(0)
    , m_Channels(0)
    , m_DecompressedAudioBuffer(nullptr)
    , m_Format(0)
    , m_Frequency(0)
{
}

AudioResourceFile::~AudioResourceFile()
{
}

TextResourceFile::TextResourceFile(const Type& type, ResourceData* resData)
    : ResourceFile(type, resData)
{
}

TextResourceFile::~TextResourceFile()
{
}

String TextResourceFile::getString() const
{
	return String(
		m_ResourceData->getRawData()->begin(), 
		m_ResourceData->getRawData()->end());
}

LuaTextResourceFile::LuaTextResourceFile(ResourceData* resData)
    : TextResourceFile(Type::LUA, resData)
{
}

LuaTextResourceFile::~LuaTextResourceFile()
{
}
