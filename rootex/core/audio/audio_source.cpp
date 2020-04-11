#include "audio_source.h"

#include "framework/systems/audio_system.h"
#include "static_audio_buffer.h"
#include "streaming_audio_buffer.h"

AudioSource::AudioSource(bool isStreaming)
    : m_IsStreaming(isStreaming)
{
	AL_CHECK(alGenSources(1, &m_SourceID));
}

AudioSource::~AudioSource()
{
	AL_CHECK(alDeleteSources(1, &m_SourceID));
}

void AudioSource::setLooping(bool enabled)
{
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

void AudioSource::pause()
{
	AL_CHECK(alSourcePause(m_SourceID));
}

void AudioSource::stop()
{
	AL_CHECK(alSourceStop(m_SourceID));	
}

bool AudioSource::isPlaying() const
{
	ALenum state;
	AL_CHECK(alGetSourcei(m_SourceID, AL_SOURCE_STATE, &state));
	return state == AL_PLAYING;
}

bool AudioSource::isPaused() const
{
	ALenum state;
	AL_CHECK(alGetSourcei(m_SourceID, AL_SOURCE_STATE, &state));
	return state == AL_PAUSED;
}

bool AudioSource::isStopped() const
{
	ALenum state;
	AL_CHECK(alGetSourcei(m_SourceID, AL_SOURCE_STATE, &state));
	return state == AL_STOPPED;
}

bool AudioSource::isLooping() const
{
	ALenum state;
	AL_CHECK(alGetSourcei(m_SourceID, AL_SOURCE_STATE, &state));
	return (state == AL_LOOPING);
}

ALuint AudioSource::getSourceID() const
{
	return m_SourceID;
}


void AudioSource::setPosition(Vector3 position) 
{
	float x = position.x;
	float y = position.y;
	float z = position.z;
	AL_CHECK(alSource3f(m_SourceID, AL_POSITION, x, y, z));
}

void AudioSource::setModel(ALfloat rolloff_factor, ALfloat reference_distance, ALfloat max_distance, String model = "linear", bool clamped = false)
{
	AL_CHECK(alSourcef(m_SourceID, AL_ROLLOFF_FACTOR, rolloff_factor));
	AL_CHECK(alSourcef(m_SourceID, AL_REFERENCE_DISTANCE, reference_distance));
	AL_CHECK(alSourcef(m_SourceID, AL_MAX_DISTANCE, max_distance));

	if (model == "linear")
	{
		if (clamped)
		{
			AL_CHECK(alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED));
		}
		else
		{
			AL_CHECK(alDistanceModel(AL_LINEAR_DISTANCE));
		}
	}
	
	if (model == "inverse")
	{
		if (clamped)
		{
			AL_CHECK(alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED));
		}
		else
		{
			AL_CHECK(alDistanceModel(AL_INVERSE_DISTANCE));
		}
	}

	if (model == "exponential")
	{
		if (clamped)
		{
			AL_CHECK(alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED));
		}
		else
		{
			AL_CHECK(alDistanceModel(AL_EXPONENT_DISTANCE));
		}
	}
}


StaticAudioSource::StaticAudioSource(Ref<StaticAudioBuffer> audio)
    : AudioSource(false)
    , m_StaticAudio(audio)
{
	AL_CHECK(alSourcei(m_SourceID, AL_BUFFER, audio->getBuffer()));
}

StaticAudioSource::~StaticAudioSource()
{
	if (isPlaying())
	{
		stop();
		unqueueBuffers();
	}
}

float StaticAudioSource::getElapsedTimeS()
{
	float pos = 0;
	AL_CHECK(alGetSourcef(m_SourceID, AL_SEC_OFFSET, &pos));
	return pos;
}

void StaticAudioSource::unqueueBuffers()
{
	AL_CHECK(alSourceUnqueueBuffers(m_SourceID, 1, &m_StaticAudio->getBuffer()));
}

float StaticAudioSource::getDuration() const
{
	return m_StaticAudio->getAudioFile()->getDuration();
}

StreamingAudioSource::StreamingAudioSource(Ref<StreamingAudioBuffer> audio)
    : AudioSource(true)
    , m_StreamingAudio(audio)
    , m_IsLooping(false)
{
	AL_CHECK(alSourceQueueBuffers(m_SourceID, m_StreamingAudio->getBufferQueueLength(), m_StreamingAudio->getBuffers()));
}

StreamingAudioSource::~StreamingAudioSource()
{
	if (isPlaying())
	{
		stop();
		unqueueBuffers();
	}
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

float StreamingAudioSource::getDuration() const
{
	return m_StreamingAudio->getAudioFile()->getDuration();
}
