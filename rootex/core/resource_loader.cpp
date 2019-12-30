#include "resource_loader.h"

#include "common/common.h"

#include "core/audio/audio_system.h"
#include "core/renderer/vertex_buffer.h"
#include "core/renderer/index_buffer.h"

HashMap<Ptr<ResourceData>, Ptr<ResourceFile>> ResourceLoader::s_ResourcesDataFiles;

TextResourceFile* ResourceLoader::CreateTextResourceFile(String path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::TXT)
		{
			return reinterpret_cast<TextResourceFile*>(item.second.get());
		}
	}

	if (OS::Exists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	FileBuffer& buffer = OS::LoadFileContents(path);
	ResourceData* resData = new ResourceData(path, buffer);
	TextResourceFile* resFile = new TextResourceFile(ResourceFile::Type::TXT, resData);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(resFile);

	return resFile;
}

LuaTextResourceFile* ResourceLoader::CreateLuaTextResourceFile(String path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::LUA)
		{
			return reinterpret_cast<LuaTextResourceFile*>(item.second.get());
		}
	}

	if (OS::Exists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	FileBuffer& buffer = OS::LoadFileContents(path);
	ResourceData* resData = new ResourceData(path, buffer);
	LuaTextResourceFile* resFile = new LuaTextResourceFile(resData);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(resFile);

	return resFile;
}

AudioResourceFile* ResourceLoader::CreateAudioResourceFile(String path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::WAV)
		{
			return reinterpret_cast<AudioResourceFile*>(item.second.get());
		}
	}

	if (OS::Exists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	const char* audioBuffer;
	int format;
	int size;
	float frequency;
	ALUT_CHECK(audioBuffer = (const char*)alutLoadMemoryFromFile(
	               OS::GetAbsolutePath(path).generic_string().c_str(),
	               &format,
	               &size,
	               &frequency));

	Vector<char> dataArray;
	dataArray.insert(
	    dataArray.begin(),
	    audioBuffer,
	    audioBuffer + size);
	ResourceData* resData = new ResourceData(path, dataArray);

	AudioResourceFile* audioRes = new AudioResourceFile(resData);
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

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(audioRes);

	return audioRes;
}

VisualModelResourceFile* ResourceLoader::CreateVisualModelResourceFile(String path)
{
	for (auto& item : s_ResourcesDataFiles)
	{
		if (item.first->getPath() == path && item.second->getType() == ResourceFile::Type::TXT)
		{
			return reinterpret_cast<VisualModelResourceFile*>(item.second.get());
		}
	}

	if (OS::Exists(path) == false)
	{
		ERR("File not found: " + path);
		return nullptr;
	}

	// File not found in cache, load it only once
	/// TODO: Add OBJ file loader
	FileBuffer& buffer = OS::LoadFileContents(path);
	Ptr<VertexBuffer> vertexBuffer(new VertexBuffer(
	    { 
			{ -1.0f, -1.0f, -1.0f },
	        { +1.0f, -1.0f, -1.0f },
	        { -1.0f, +1.0f, -1.0f },
	        { +1.0f, +1.0f, -1.0f },
	        { -1.0f, -1.0f, +1.0f },
	        { +1.0f, -1.0f, +1.0f },
	        { -1.0f, +1.0f, +1.0f },
	        { +1.0f, +1.0f, +1.0f } 
		}));
	Ptr<IndexBuffer> indexBuffer(new IndexBuffer({
		2, 0, 1,
		3, 2, 1,
		3, 1, 5,
		7, 3, 5,
		6, 2, 3,
		6, 3, 7,
		5, 4, 7,
		7, 4, 6,
		4, 0, 2,
		4, 2, 6,
		1, 0, 4,
		5, 1, 4 }));
	ResourceData* resData = new ResourceData(path, buffer);
	VisualModelResourceFile* resFile = new VisualModelResourceFile(std::move(vertexBuffer), std::move(indexBuffer), resData);

	s_ResourcesDataFiles[Ptr<ResourceData>(resData)] = Ptr<ResourceFile>(resFile);

	return resFile;
}
