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

	AudioSource(bool isStreaming);
	~AudioSource();

public:
	virtual void setLooping(bool enabled);
	virtual void queueNewBuffers();

	void play();

	bool isLooping();
	ALuint getSourceID();
};

class StaticAudioSource : public AudioSource
{
	StaticAudioBuffer* m_StaticAudio;

public:
	StaticAudioSource(StaticAudioBuffer* audio);
	~StaticAudioSource();
};

class StreamingAudioSource : public AudioSource
{
	StreamingAudioBuffer* m_StreamingAudio;

public:
	StreamingAudioSource(StreamingAudioBuffer* audio);
	~StreamingAudioSource();

	void setLooping(bool enabled) override;
	void queueNewBuffers() override;
};
