#include "resource_loader.h"

#include "audio/audio_system.h"
#include "common/common.h"
#include "resource_data_reserve.h"

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
