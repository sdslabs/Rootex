#include "audio_system.h"
#include "audio_source.h"
#include "core/resource_data.h"

String AudioSystem::GetOpenALErrorString(int errID)
{
	switch (errID)
	{
	case AL_NO_ERROR:
		return "OK";
	case AL_INVALID_NAME:
		return "Invalid Name";
	case AL_INVALID_ENUM:
		return "Invalid Enum";
	case AL_INVALID_VALUE:
		return "Invalid Value";
	case AL_INVALID_OPERATION:
		return "Invalid Operation";
	case AL_OUT_OF_MEMORY:
		return "Out of Memory";
	}
	return "Unknown Error: " + std::to_string(errID);
}

void AudioSystem::CheckOpenALError(const char* msg, const char* fname, int line)
{
	ALenum err = alGetError();
	if (err != AL_NO_ERROR)
	{
		ERR("OpenAL error (" + std::to_string(err) + "): " + GetOpenALErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
	}
}

void AudioSystem::CheckALUTError(const char* msg, const char* fname, int line)
{
	ALenum err = alutGetError();
	if (err != ALUT_ERROR_NO_ERROR)
	{
		ERR("ALUT error (" + std::to_string(err) + "): " + alutGetErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
	}
}

void AudioSystem::play(AudioSource* source)
{
	alSourcePlay(source->m_SourceID);
}

bool AudioSystem::initialize()
{

	alGetError();

	const char* ALDevice = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	m_Device = alcOpenDevice(ALDevice);

	if (m_Device == nullptr)
	{
		ERR("AudioManager: Failed to Initialize OpenAL device");
		return false;
	}

	m_Context = alcCreateContext(m_Device, nullptr);
	AL_CHECK(alcMakeContextCurrent(m_Context));

	if (!alutInitWithoutContext(NULL, NULL))
	{
		ERR("AudioManager: ALUT failed to initialize");
		return false;
	}

	return true;
}

AudioSystem* AudioSystem::GetSingleton()
{
	static AudioSystem singleton;

	return &singleton;
}

void AudioSystem::shutDown()
{
	alcCloseDevice(m_Device);

	PANIC(m_Buffers.size() > 0, "AL buffers are still alive. Deleting leaked buffers.");

	for (auto& it : m_Buffers)
	{
		AL_CHECK(alDeleteBuffers(1, &it));
	}

	alutExit();
}

ALuint AudioSystem::makeBuffer(ResourceFile* audioFile)
{
	PANIC(audioFile->getType() != ResourceFile::Type::WAV, "AudioSystem: Trying to load a non-WAV file in a sound buffer");

	ALuint bufferID;
	//AL_CHECK(bufferID = alutLoadMemoryFromFileImage(audioFile->getData()->getRawData().data(), audioFile->getData()->getRawDataByteSize()));
	AL_CHECK(bufferID = alutCreateBufferHelloWorld());

	m_Buffers.push_back(bufferID);

	return bufferID;
}

void AudioSystem::destroyBuffer(AudioBuffer* buffer)
{
	ALuint bufferID = buffer->m_BufferID;

	AL_CHECK(alDeleteBuffers(1, &bufferID));

	m_Buffers.erase(std::find(m_Buffers.begin(), m_Buffers.end(), bufferID));
}

ALuint AudioSystem::makeSource()
{
	ALuint source;
	AL_CHECK(alGenSources(1, &source));
	
	return source;
}

void AudioSystem::attach(AudioSource* source, AudioBuffer* audio)
{
	alSourcei(source->m_SourceID, AL_BUFFER, audio->m_BufferID);
}

void AudioSystem::deleteSource(AudioSource* source)
{
	alDeleteSources(1, &source->m_SourceID);
}

AudioSystem::AudioSystem()
    : m_Context(nullptr)
    , m_Device(nullptr)
{
}

AudioSystem::~AudioSystem()
{
	shutDown();
}
