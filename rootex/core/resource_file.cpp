#include "resource_file.h"

#include <bitset>
#include <sstream>

#include "resource_loader.h"
#include "audio/audio_system.h"
#include "renderer/rendering_device.h"

ResourceFile::ResourceFile(const Type& type, ResourceData* resData)
    : m_Type(type)
    , m_ResourceData(resData)
{
	PANIC(resData == nullptr, "Null resource found. Resource of this type has not been loaded correctly: " + std::to_string((int)type));
	m_LastReadTime = OS::s_FileSystemClock.now();
	m_LastChangedTime = OS::GetFileLastChangedTime(getPath().string());
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

void ResourceFile::reload()
{
	m_LastReadTime = OS::s_FileSystemClock.now();
	m_LastChangedTime = OS::GetFileLastChangedTime(getPath().string());
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
    : ResourceFile(Type::Wav, resData)
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

void AudioResourceFile::reload()
{
	ResourceFile::reload();
	const char* audioBuffer;
	int format;
	int size;
	float frequency;
	ALUT_CHECK(audioBuffer = (const char*)alutLoadMemoryFromFile(
	               OS::GetAbsolutePath(m_ResourceData->getPath().string()).string().c_str(),
	               &format,
	               &size,
	               &frequency));

	m_ResourceData->getRawData()->clear();
	m_ResourceData->getRawData()->insert(
	    m_ResourceData->getRawData()->begin(),
	    audioBuffer,
	    audioBuffer + size);

	AudioResourceFile* audioRes = this;
	audioRes->m_DecompressedAudioBuffer = audioBuffer;
	audioRes->m_Format = format;
	audioRes->m_AudioDataSize = size;
	audioRes->m_Frequency = frequency;

	switch (audioRes->getFormat())
	{
	case AL_FORMAT_MONO8:
		audioRes->m_Channels = 1;
		audioRes->m_BitDepth = 8;
		break;

	case AL_FORMAT_MONO16:
		audioRes->m_Channels = 1;
		audioRes->m_BitDepth = 16;
		break;

	case AL_FORMAT_STEREO8:
		audioRes->m_Channels = 2;
		audioRes->m_BitDepth = 8;
		break;

	case AL_FORMAT_STEREO16:
		audioRes->m_Channels = 2;
		audioRes->m_BitDepth = 16;
		break;

	default:
		ERR("Unknown channels and bit depth in WAV data");
	}

	audioRes->m_Duration = size * 8 / (audioRes->m_Channels * audioRes->m_BitDepth);
	audioRes->m_Duration /= frequency;
}

TextResourceFile::TextResourceFile(const Type& type, ResourceData* resData)
    : ResourceFile(type, resData)
{
}

TextResourceFile::~TextResourceFile()
{
}

void TextResourceFile::popBack()
{
	m_ResourceData->getRawData()->pop_back();
}

void TextResourceFile::append(const String& add)
{
	m_ResourceData->getRawData()->insert(m_ResourceData->getRawData()->end(), add.begin(), add.end());
}

void TextResourceFile::reload()
{
	ResourceFile::reload();
	ResourceLoader::ReloadResourceData(m_ResourceData->getPath().string());
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

VisualModelResourceFile::VisualModelResourceFile(Ptr<VertexBuffer> vertexBuffer, Ptr<IndexBuffer> indexBuffer, ResourceData* resData)
    : ResourceFile(Type::Obj, resData)
    , m_VertexBuffer(std::move(vertexBuffer))
    , m_IndexBuffer(std::move(indexBuffer))
{
}

VisualModelResourceFile::~VisualModelResourceFile()
{
}

void VisualModelResourceFile::reload()
{
	ResourceFile::reload();
	PANIC(ResourceLoader::GetModelLoader().LoadFile(OS::GetAbsolutePath(m_ResourceData->getPath().string()).generic_string()) == false, "Model could not be loaded: " + OS::GetAbsolutePath(m_ResourceData->getPath().string()).generic_string());

	VertexData vertex;

	Vector<VertexData> vertices;
	vertices.reserve(ResourceLoader::GetModelLoader().LoadedVertices.size());

	for (auto& v : ResourceLoader::GetModelLoader().LoadedVertices)
	{
		vertex.m_Position.x = v.Position.X;
		vertex.m_Position.y = v.Position.Y;
		vertex.m_Position.z = v.Position.Z;
		vertex.m_Normal.x = v.Normal.X;
		vertex.m_Normal.y = v.Normal.Y;
		vertex.m_Normal.z = v.Normal.Z;
		vertex.m_TextureCoord.x = v.TextureCoordinate.X;
		vertex.m_TextureCoord.y = v.TextureCoordinate.Y;
		vertices.push_back(vertex);
	}

	ResourceLoader::ReloadResourceData(m_ResourceData->getPath().string());
	m_VertexBuffer.reset(new VertexBuffer(vertices));
	m_IndexBuffer.reset(new IndexBuffer(ResourceLoader::GetModelLoader().LoadedIndices));
}

ImageResourceFile::ImageResourceFile(ResourceData* resData)
    : ResourceFile(Type::Image, resData)
{
}

ImageResourceFile::~ImageResourceFile()
{
}

void ImageResourceFile::reload()
{
	ResourceFile::reload();
	ResourceLoader::ReloadResourceData(m_ResourceData->getPath().string());
}

FontResourceFile::FontResourceFile(const String& name, ResourceData* resData)
    : ResourceFile(Type::Font, resData)
    , m_Name(name)
{
	m_Font = RenderingDevice::GetSingleton()->createFont(resData->getRawData());
	m_Font->SetDefaultCharacter('X');
}

FontResourceFile::~FontResourceFile()
{
}

void FontResourceFile::reload()
{
	ResourceFile::reload();
	ResourceLoader::ReloadResourceData(m_ResourceData->getPath().string());
}
