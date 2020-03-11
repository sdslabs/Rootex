#pragma once

class StreamingAudioBuffer;
class StaticAudioBuffer;

typedef unsigned int ALuint;

#define MIN_TO_S 60.0f

class AudioSource
{
protected:
	ALuint m_SourceID;

	bool m_IsStreaming;

	AudioSource(bool isStreaming);
	virtual ~AudioSource();

public:
	virtual void setLooping(bool enabled);
	virtual void queueNewBuffers();

	void play();
	void pause();
	void stop();

	bool isPlaying() const;
	bool isPaused() const;
	bool isStopped() const;
	bool isLooping() const;
	ALuint getSourceID() const;
	virtual float getDuration() const = 0;

};

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
