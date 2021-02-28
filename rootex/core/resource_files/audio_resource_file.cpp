#include "audio_resource_file.h"

#include "systems/audio_system.h"

AudioResourceFile::AudioResourceFile(const FilePath& path)
    : ResourceFile(Type::Audio, path)
    , m_AudioDataSize(0)
    , m_BitDepth(0)
    , m_Channels(0)
    , m_DecompressedAudioBuffer(nullptr)
    , m_Format(0)
    , m_Frequency(0)
{
	reimport();
}

AudioResourceFile::~AudioResourceFile()
{
	resetBuffer();
}

void AudioResourceFile::resetBuffer()
{
	if (m_DecompressedAudioBuffer)
	{
		ALUT_CHECK(alutUnloadWAV(m_Format, (void*)m_DecompressedAudioBuffer, m_AudioDataSize, m_Frequency));
		;
		m_DecompressedAudioBuffer = nullptr;
	}
}

void AudioResourceFile::reimport()
{
	ResourceFile::reimport();

	resetBuffer();

	const char* audioBuffer;
	int format;
	int size;
	float frequency;

	// We need to manually destruct this buffer on destruction.
	ALUT_CHECK(audioBuffer = (const char*)alutLoadMemoryFromFile(
	               OS::GetAbsolutePath(m_Path.generic_string()).generic_string().c_str(),
	               &format,
	               &size,
	               &frequency));

	m_DecompressedAudioBuffer = audioBuffer;
	m_Format = format;
	m_AudioDataSize = size;
	m_Frequency = frequency;

	switch (m_Format)
	{
	case AL_FORMAT_MONO8:
		m_Channels = 1;
		m_BitDepth = 8;
		break;

	case AL_FORMAT_MONO16:
		m_Channels = 1;
		m_BitDepth = 16;
		break;

	case AL_FORMAT_STEREO8:
		m_Channels = 2;
		m_BitDepth = 8;
		break;

	case AL_FORMAT_STEREO16:
		m_Channels = 2;
		m_BitDepth = 16;
		break;

	default:
		ERR("Unknown channels and bit depth in WAV data");
	}

	m_Duration = size * 8 / (m_Channels * m_BitDepth);
	m_Duration /= frequency;
}
