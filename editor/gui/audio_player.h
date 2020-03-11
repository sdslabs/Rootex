#pragma once

#include "common/common.h"
#include "resource_loader.h"
#include "audio/static_audio_buffer.h"
#include "audio/audio_source.h"
#include "os/timer.h"

class AudioPlayer
{
	float m_FractionProgress;
	AudioResourceFile* m_OpenFile = nullptr;
	Ref<StaticAudioBuffer> m_Buffer;
	Ref<StaticAudioSource> m_Source;
	bool m_Looping = false;

	StopTimer m_Timer;

	void drawFileInfo();

public:
	AudioPlayer();
	AudioPlayer(AudioPlayer&) = delete;
	~AudioPlayer() = default;

	ResourceFile* load(const FilePath& filePath);
	void unload();
	void draw();
};
