#include "resource_file.h"

#include <bitset>
#include <sstream>

#include "resource_loader.h"
#include "framework/systems/audio_system.h"
#include "renderer/rendering_device.h"
#include "interpreter.h"

ResourceFile::ResourceFile(const Type& type, ResourceData* resData)
    : m_Type(type)
    , m_ResourceData(resData)
{
	PANIC(resData == nullptr, "Null resource found. Resource of this type has not been loaded correctly: " + std::to_string((int)type));
	m_LastReadTime = OS::s_FileSystemClock.now();
	m_LastChangedTime = OS::GetFileLastChangedTime(getPath().string());
}

void ResourceFile::RegisterAPI(sol::state& rootex)
{
	sol::usertype<ResourceFile> resourceFile = rootex.new_usertype<ResourceFile>("ResourceFile");
	resourceFile["isValid"] = &ResourceFile::isValid;
	resourceFile["isDirty"] = &ResourceFile::isDirty;
	resourceFile["isOpen"] = &ResourceFile::isOpen;
	resourceFile["getPath"] = [](ResourceFile& f) { return f.getPath().string(); };
	resourceFile["getType"] = &ResourceFile::getType;
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

FilePath ResourceFile::getPath() const
{
	return m_ResourceData->getPath();
}

ResourceFile::Type ResourceFile::getType() const
{
	return m_Type;
}

ResourceData* ResourceFile::getData()
{
	return m_ResourceData;
}

const FileTimePoint& ResourceFile::getLastChangedTime()
{
	m_LastChangedTime = OS::GetFileLastChangedTime(getPath().string());
	return m_LastChangedTime;
}

bool ResourceFile::isDirty()
{
	return getLastReadTime() < getLastChangedTime();
}

AudioResourceFile::AudioResourceFile(ResourceData* resData)
    : ResourceFile(Type::Audio, resData)
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

void AudioResourceFile::RegisterAPI(sol::state& rootex)
{
	sol::usertype<AudioResourceFile> audioResourceFile = rootex.new_usertype<AudioResourceFile>(
	    "AudioResourceFile",
	    sol::base_classes, sol::bases<ResourceFile>());
	audioResourceFile["getAudioDataSize"] = &AudioResourceFile::getAudioDataSize;
	audioResourceFile["getFormat"] = &AudioResourceFile::getFormat;
	audioResourceFile["getFrequency"] = &AudioResourceFile::getFrequency;
	audioResourceFile["getBitDepth"] = &AudioResourceFile::getBitDepth;
	audioResourceFile["getChannels"] = &AudioResourceFile::getChannels;
	audioResourceFile["getDuration"] = &AudioResourceFile::getDuration;
}

TextResourceFile::TextResourceFile(const Type& type, ResourceData* resData)
    : ResourceFile(type, resData)
{
}

TextResourceFile::~TextResourceFile()
{
}

void TextResourceFile::RegisterAPI(sol::state& rootex)
{
	sol::usertype<TextResourceFile> textResourceFile = rootex.new_usertype<TextResourceFile>(
	    "TextResourceFile",
	    sol::base_classes, sol::bases<ResourceFile>());
	textResourceFile["getString"] = &TextResourceFile::getString;
}

void TextResourceFile::putString(const String& newData)
{
	*m_ResourceData->getRawData() = FileBuffer(newData.begin(), newData.end());
}

void TextResourceFile::popBack()
{
	m_ResourceData->getRawData()->pop_back();
}

void TextResourceFile::append(const String& add)
{
	m_ResourceData->getRawData()->insert(m_ResourceData->getRawData()->end(), add.begin(), add.end());
}

String TextResourceFile::getString() const
{
	return String(
	    m_ResourceData->getRawData()->begin(),
	    m_ResourceData->getRawData()->end());
}

LuaTextResourceFile::LuaTextResourceFile(ResourceData* resData)
    : TextResourceFile(Type::Lua, resData)
{
}

LuaTextResourceFile::~LuaTextResourceFile()
{
}

void LuaTextResourceFile::RegisterAPI(sol::state& rootex)
{
	sol::usertype<LuaTextResourceFile> luaTextResourceFile = rootex.new_usertype<LuaTextResourceFile>(
	    "LuaTextResourceFile",
	    sol::base_classes, sol::bases<ResourceFile, TextResourceFile>());
}

ModelResourceFile::ModelResourceFile(ResourceData* resData)
    : ResourceFile(Type::Model, resData)
{
}

ModelResourceFile::~ModelResourceFile()
{
}

void ModelResourceFile::RegisterAPI(sol::state& rootex)
{
	sol::usertype<ModelResourceFile> modelResourceFile = rootex.new_usertype<ModelResourceFile>(
	    "ModelResourceFile",
	    sol::base_classes, sol::bases<ResourceFile>());
}

ImageResourceFile::ImageResourceFile(ResourceData* resData)
    : ResourceFile(Type::Image, resData)
{
}

ImageResourceFile::~ImageResourceFile()
{
}

void ImageResourceFile::RegisterAPI(sol::state& rootex)
{
	sol::usertype<ImageResourceFile> imageResourceFile = rootex.new_usertype<ImageResourceFile>(
	    "ImageResourceFile",
	    sol::base_classes, sol::bases<ResourceFile>());
}

FontResourceFile::FontResourceFile(ResourceData* resData)
    : ResourceFile(Type::Font, resData)
{
	regenerateFont();
}

FontResourceFile::~FontResourceFile()
{
}

void FontResourceFile::regenerateFont()
{
	m_Font = RenderingDevice::GetSingleton()->createFont(m_ResourceData->getRawData());
	m_Font->SetDefaultCharacter('X');
}

void FontResourceFile::RegisterAPI(sol::state& rootex)
{
	sol::usertype<FontResourceFile> fontResourceFile = rootex.new_usertype<FontResourceFile>(
	    "FontResourceFile",
	    sol::base_classes, sol::bases<ResourceFile>());
}
