#include "audio_source.h"

#include "audio_system.h"
#include "static_audio_buffer.h"
#include "streaming_audio_buffer.h"

AudioSource::AudioSource(bool isStreaming)
    : m_IsStreaming(isStreaming)
    , m_IsLooping(false)
    , m_IsPaused(true)
    , m_IsPlaying(false)
{
	AudioSystem::GetSingleton()->registerInstance(this);
	AL_CHECK(alGenSources(1, &m_SourceID));
}

AudioSource::~AudioSource()
{
	AudioSystem::GetSingleton()->deregisterInstance(this);
	AL_CHECK(alDeleteSources(1, &m_SourceID));
}

void AudioSource::setLooping(bool enabled)
{
	m_IsLooping = enabled;
	AL_CHECK(alSourcei(m_SourceID, AL_LOOPING, enabled));
}

void AudioSource::queueNewBuffers()
{
	// Empty
}

void AudioSource::play()
{
	AL_CHECK(alSourcePlay(m_SourceID));
	m_IsPlaying = true;
	m_IsPaused = false;
}

void AudioSource::pause()
{
	AL_CHECK(alSourcePause(m_SourceID));
	m_IsPaused = true;	
}

void AudioSource::stop()
{
	if (isPlaying())
	{
		pause();
		AL_CHECK(alSourceStop(m_SourceID));
		m_IsPlaying = false;
		m_IsPaused = true;
	}
}

bool AudioSource::isPlaying() const
{
	return m_IsPlaying;
}

bool AudioSource::isPaused() const
{
	return m_IsPaused;
}

bool AudioSource::isLooping() const
{
	return m_IsLooping;
}

ALuint AudioSource::getSourceID() const
{
	return m_SourceID;
}

StaticAudioSource::StaticAudioSource(Ref<StaticAudioBuffer> audio)
    : AudioSource(false)
    , m_StaticAudio(audio)
{
	AL_CHECK(alSourcei(m_SourceID, AL_BUFFER, audio->getBuffer()));
}

StaticAudioSource::~StaticAudioSource()
{
	unqueueBuffers();
}

void StaticAudioSource::unqueueBuffers()
{
	AL_CHECK(alSourceUnqueueBuffers(m_SourceID, 1, &m_StaticAudio->getBuffer()));
}

StreamingAudioSource::StreamingAudioSource(Ref<StreamingAudioBuffer> audio)
    : AudioSource(true)
    , m_StreamingAudio(audio)
{
	AL_CHECK(alSourceQueueBuffers(m_SourceID, m_StreamingAudio->getBufferQueueLength(), m_StreamingAudio->getBuffers()));
}

StreamingAudioSource::~StreamingAudioSource()
{
	unqueueBuffers();
}

void StreamingAudioSource::setLooping(bool enabled)
{
	m_IsLooping = enabled;
}

void StreamingAudioSource::queueNewBuffers()
{
	int numUsedUp;
	AL_CHECK(alGetSourcei(m_SourceID, AL_BUFFERS_PROCESSED, &numUsedUp));

	if (numUsedUp > 0)
	{
		AL_CHECK(alSourceUnqueueBuffers(m_SourceID, numUsedUp, m_StreamingAudio->getBuffers()));
		m_StreamingAudio->loadNewBuffers(numUsedUp, isLooping());
		AL_CHECK(alSourceQueueBuffers(m_SourceID, numUsedUp, m_StreamingAudio->getBuffers()));

		static ALint val;
		AL_CHECK(alGetSourcei(m_SourceID, AL_SOURCE_STATE, &val));
		if (val != AL_PLAYING)
		{
			AL_CHECK(alSourcePlay(m_SourceID));
		}
	}
}

void StreamingAudioSource::unqueueBuffers()
{
	AL_CHECK(alSourceUnqueueBuffers(m_SourceID, BUFFER_COUNT, m_StreamingAudio->getBuffers()));
}
