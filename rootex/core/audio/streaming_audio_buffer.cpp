#include "streaming_audio_buffer.h"

#include "audio_system.h"
#include "resource_data.h"
#include "resource_file.h"

bool isBigEndian()
{
	int a = 1;
	return !((char*)&a)[0];
}

int convertToInt(char* buffer, int len)
{
	int a = 0;
	if (!isBigEndian())
		for (int i = 0; i < len; i++)
			((char*)&a)[i] = buffer[i];
	else
		for (int i = 0; i < len; i++)
			((char*)&a)[3 - i] = buffer[i];
	return a;
}

char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size)
{
	// This is from: https://stackoverflow.com/a/52173552/7370948
	char buffer[4];
	std::ifstream in(fn, std::ios::binary);
	in.read(buffer, 4);
	if (strncmp(buffer, "RIFF", 4) != 0)
	{
		std::cout << "this is not a valid WAVE file" << std::endl;
		return NULL;
	}
	in.read(buffer, 4);
	in.read(buffer, 4); //WAVE
	in.read(buffer, 4); //fmt
	in.read(buffer, 4); //16
	in.read(buffer, 2); //1
	in.read(buffer, 2);
	chan = convertToInt(buffer, 2);
	in.read(buffer, 4);
	samplerate = convertToInt(buffer, 4);
	in.read(buffer, 4);
	in.read(buffer, 2);
	in.read(buffer, 2);
	bps = convertToInt(buffer, 2);
	in.read(buffer, 4); //data
	in.read(buffer, 4);
	size = convertToInt(buffer, 4);
	char* data = new char[size];
	in.read(data, size);
	return data;
}

void StreamingAudioBuffer::initializeBuffers()
{
	PANIC(m_AudioFile->getType() != ResourceFile::Type::WAV, "AudioSystem: Trying to load a non-WAV file in a sound buffer");

	ResourceData* fileStream = m_AudioFile->getData();
	FileBuffer* fileBuffer = fileStream->getRawData();

	/*m_DecompressedFileBuffer = loadWAV(m_AudioFile->getPath().c_str(), m_Channels, m_Frequency, m_BitDepth, m_AudioDataSize);

	if (m_Channels == 1)
	{
		if (m_BitDepth == 8)
		{
			m_Format = AL_FORMAT_MONO8;
		}
		else
		{
			m_Format = AL_FORMAT_MONO16;
		}
	}
	else
	{
		if (m_BitDepth == 8)
		{
			m_Format = AL_FORMAT_STEREO8;
		}
		else
		{
			m_Format = AL_FORMAT_STEREO16;
		}
	}*/

	m_DecompressedAudioBuffer = (const char *)alutLoadMemoryFromFile(m_AudioFile->getPath().c_str(), &m_Format, &m_AudioDataSize, &m_Frequency);

	AL_CHECK(alGenBuffers(BUFFER_COUNT, m_Buffers));
	
	int m_BufferSize = m_AudioDataSize / BUFFER_COUNT;
	m_BufferCursor = m_DecompressedAudioBuffer;
	m_BufferEnd = m_BufferCursor + m_AudioDataSize;

	int i = 0;
	while (m_BufferCursor < (m_BufferEnd - m_BufferSize))
	{
		AL_CHECK(alBufferData(m_Buffers[i], m_Format, m_BufferCursor, m_BufferSize, (ALsizei)m_Frequency));
		
		m_BufferCursor += m_BufferSize;
		i++;
	}
}

void StreamingAudioBuffer::destroyBuffers()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		AL_CHECK(alDeleteBuffers(1, &m_Buffers[i]));
	}

	delete[] m_DecompressedAudioBuffer;
}

void StreamingAudioBuffer::loadNewBuffers(int count, bool isLooping)
{
	for (int i = 0; i < count; i++)
	{
		m_BufferEnd = m_BufferCursor + m_BufferSize;

		if (m_BufferCursor == m_DecompressedAudioBuffer + m_AudioDataSize) // Data has exhausted
		{
			if (isLooping) // Re-queue if looping
			{
				m_BufferCursor = m_DecompressedAudioBuffer;
			}
		}
		
		if (m_BufferEnd >= m_DecompressedAudioBuffer + m_AudioDataSize) // Data not left enough to entirely fill the next buffer
		{
			m_BufferEnd = m_DecompressedAudioBuffer + m_AudioDataSize; // Only take what you can
		}

		AL_CHECK(alBufferData(m_Buffers[i], m_Format, m_BufferCursor, m_BufferEnd - m_BufferCursor, m_Frequency));
		m_BufferCursor = m_BufferEnd;	
	}
}

StreamingAudioBuffer::StreamingAudioBuffer(Ref<ResourceFile> audioFile)
    : AudioBuffer(audioFile)
{
	initializeBuffers();
}

StreamingAudioBuffer::~StreamingAudioBuffer()
{
	destroyBuffers();
}

ALuint* StreamingAudioBuffer::getBuffers()
{
	return m_Buffers;
}

int StreamingAudioBuffer::getFrequency()
{
	return m_Frequency;
}

int StreamingAudioBuffer::getBitDepth()
{
	return m_BitDepth;
}

int StreamingAudioBuffer::getChannels()
{
	return m_Channels;
}
