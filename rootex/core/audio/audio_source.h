#pragma once

class AudioStreamingBuffer;
class AudioStaticBuffer;

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
	void setLooping(bool enabled);
	virtual void queueNewBuffers();
	
	void play();
	
	bool isLooping();
	ALuint getSourceID();
};

class StaticAudioSource : public AudioSource
{
	AudioStaticBuffer* m_StaticAudio;

public:
	StaticAudioSource(AudioStaticBuffer* audio);
	~StaticAudioSource();
};

class StreamingAudioSource : public AudioSource
{
	AudioStreamingBuffer* m_StreamingAudio;

public:
	StreamingAudioSource(AudioStreamingBuffer* audio);
	~StreamingAudioSource();

	void queueNewBuffers() override;
};
