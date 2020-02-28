#pragma once

class StreamingAudioBuffer;
class StaticAudioBuffer;

typedef unsigned int ALuint;

class AudioSource
{
protected:
	ALuint m_SourceID;

	bool m_IsStreaming;
	bool m_IsLooping;
	bool m_IsPaused;
	bool m_IsPlaying;

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
	bool isLooping() const;
	ALuint getSourceID() const;
};

class StaticAudioSource : public AudioSource
{
	Ref<StaticAudioBuffer> m_StaticAudio;

public:
	StaticAudioSource(Ref<StaticAudioBuffer> audio);
	~StaticAudioSource();

	void unqueueBuffers();
};

class StreamingAudioSource : public AudioSource
{
	Ref<StreamingAudioBuffer> m_StreamingAudio;

public:
	StreamingAudioSource(Ref<StreamingAudioBuffer> audio);
	~StreamingAudioSource();

	void setLooping(bool enabled) override;
	void queueNewBuffers() override;
	void unqueueBuffers();
};
