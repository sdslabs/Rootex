#include "audio_source.h"

#include "audio_system.h"
#include "audio_static_buffer.h"
#include "audio_streaming_buffer.h"

AudioSource::AudioSource(bool isStreaming)
    : m_IsStreaming(isStreaming)
    , m_IsLooping(false)
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
}

bool AudioSource::isLooping()
{
	return m_IsLooping;
}

ALuint AudioSource::getSourceID()
{
	return m_SourceID;
}

StaticAudioSource::StaticAudioSource(AudioStaticBuffer* audio)
    : AudioSource(false)
    , m_StaticAudio(audio)
{
	AL_CHECK(alSourcei(m_SourceID, AL_BUFFER, audio->getBuffer()));
}

StaticAudioSource::~StaticAudioSource()
{
}

StreamingAudioSource::StreamingAudioSource(AudioStreamingBuffer* audio)
    : AudioSource(true)
    , m_StreamingAudio(audio)
{
	AudioSystem::GetSingleton()->registerInstance(this);
	AL_CHECK(alSourceQueueBuffers(m_SourceID, BUFFER_COUNT, audio->getBuffers()));
}

StreamingAudioSource::~StreamingAudioSource()
{
	AudioSystem::GetSingleton()->deregisterInstance(this);
}

void StreamingAudioSource::queueNewBuffers()
{
	int numUsedUp;
	AL_CHECK(alGetSourcei(m_SourceID, AL_BUFFERS_PROCESSED, &numUsedUp));
	
	if (numUsedUp > 1)
	{
		AL_CHECK(alSourceUnqueueBuffers(m_SourceID, numUsedUp, m_StreamingAudio->getBuffers()));
		m_StreamingAudio->loadNewBuffers(numUsedUp, isLooping());
		AL_CHECK(alSourceQueueBuffers(m_SourceID, numUsedUp, m_StreamingAudio->getBuffers()));
	}
}
