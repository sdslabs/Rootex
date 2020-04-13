#pragma once

class StreamingAudioBuffer;
class StaticAudioBuffer;

typedef unsigned int ALuint;
typedef float ALfloat; // doubt(32-bit IEEE 754 floating-point)

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

	enum class AttenuationModel
	{
		linear,
		inverse,
		exponential,
		linearClamped,
		inverseClamped,
		exponentialClamped
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

	void setPosition(Vector3 position);
	void setModel(AttenuationModel model = AttenuationModel::linear);
	void setRolloffFactor(ALfloat rolloffFactor);
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
