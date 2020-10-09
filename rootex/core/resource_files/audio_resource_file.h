#pragma once

#include "resource_file.h"

#include "al.h"
#include "alut.h"

/// Representation of an audio file. Only .wav files are supported.
class AudioResourceFile : public ResourceFile
{
	ALenum m_Format;
	float m_Frequency;
	int m_BitDepth;
	int m_Channels;
	float m_Duration;

	const char* m_DecompressedAudioBuffer;
	ALsizei m_AudioDataSize;

	explicit AudioResourceFile(const FilePath& path);

	friend class ResourceLoader;

	void resetBuffer();

public:
	static void RegisterAPI(sol::table& rootex);

	explicit AudioResourceFile(AudioResourceFile&) = delete;
	explicit AudioResourceFile(AudioResourceFile&&) = delete;
	~AudioResourceFile();

	void reimport() override;

	const char* getAudioData() const { return m_DecompressedAudioBuffer; }
	/// Get size of decompressed audio data.
	ALsizei getAudioDataSize() const { return m_AudioDataSize; }
	/// Returns the same enum value that OpenAL uses.
	ALenum getFormat() const { return m_Format; }
	float getFrequency() const { return m_Frequency; }
	int getBitDepth() const { return m_BitDepth; }
	int getChannels() const { return m_Channels; }
	/// Get the duration of the audio in seconds.
	float getDuration() const { return m_Duration; }
};
