#pragma once
#include "rootex\vendor\OpenAL\include\al.h""

class StreamingAudioBuffer;
class StaticAudioBuffer;

typedef unsigned int ALuint;
typedef float ALfloat; 

/// Convert minutes to seconds
#define MIN_TO_S 60.0f

/// An interface for an audio source in the game world.
class AudioSource
{
protected:
	ALuint m_SourceID;

	/// RTTI for storing if the audio buffer is being streamed
	bool m_IsStreaming;
	AudioSource(bool isStreaming);
	virtual ~AudioSource();

public:

	/// Defines all attenuation models provided by OpenAL
	enum class AttenuationModel
	{
		Linear = AL_LINEAR_DISTANCE,
		Inverse = AL_INVERSE_DISTANCE,
		Exponential = AL_EXPONENT_DISTANCE,
		LinearClamped = AL_LINEAR_DISTANCE_CLAMPED,
		InverseClamped = AL_INVERSE_DISTANCE_CLAMPED,
		ExponentialClamped = AL_EXPONENT_DISTANCE_CLAMPED
	};

	virtual void setLooping(bool enabled);
	/// Queue new buffers to the audio card if possible.
	virtual void queueNewBuffers();

	void play();
	void pause();
	void stop();

	bool isPlaying() const;
	bool isPaused() const;
	bool isStopped() const;
	bool isLooping() const;
	ALuint getSourceID() const;
	/// Get audio duration in seconds.
	virtual float getDuration() const = 0;

	void setPosition(Vector3& position);
	void setModel(AttenuationModel distanceModel);
	/// Roll Off Factor: The rate of change of attenuation
	void setRollOffFactor(ALfloat rolloffFactor);
	/// Reference Distance: Distance until which clamping occurs
	void setReferenceDistance(ALfloat referenceDistance);
	void setMaxDistance(ALfloat maxDistance);
};

/// An audio source that uses StaticAudioBuffer.
class StaticAudioSource : public AudioSource
{
	Ref<StaticAudioBuffer> m_StaticAudio;

public:
	StaticAudioSource(Ref<StaticAudioBuffer> audio);
	~StaticAudioSource();

	void unqueueBuffers();

	virtual float getDuration() const override;
	float getElapsedTimeS();
};

/// An audio source that uses StreamingAudioBuffer.
class StreamingAudioSource : public AudioSource
{
	Ref<StreamingAudioBuffer> m_StreamingAudio;

	bool m_IsLooping;

public:
	StreamingAudioSource(Ref<StreamingAudioBuffer> audio);
	~StreamingAudioSource();

	void setLooping(bool enabled) override;
	void queueNewBuffers() override;
	void unqueueBuffers();

	virtual float getDuration() const override;
};
