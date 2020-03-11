#include "audio_player.h"

#include "audio/audio_system.h"

#include "imgui.h"

AudioPlayer::AudioPlayer() 
{
}

void AudioPlayer::load(const FilePath& filePath)
{
	m_FractionProgress = 0.0f;
	m_OpenFile = ResourceLoader::CreateAudioResourceFile(filePath.string());
	
	m_Buffer.reset(new StaticAudioBuffer(m_OpenFile));
	m_Source.reset(new StaticAudioSource(m_Buffer));

	PANIC(m_OpenFile == nullptr, "Audio file could not be loaded");
}

void AudioPlayer::unload()
{
	if (m_Source && m_Buffer)
	{
		if (m_Source->isPlaying())
		{
			m_Source->stop();
		}
		m_Timer.reset();
		m_Source.reset();
		m_Buffer.reset();
	}
}

void AudioPlayer::draw()
{
	if (ImGui::Button("Play"))
	{
		if (!m_Source->isPlaying())
		{
			m_Source->play();
			m_Timer.reset();
		}
	}
	
	ImGui::SameLine();
	if (m_Source->isPlaying())
	{
		m_FractionProgress = m_Timer.getTimeMs() * MS_TO_S / m_Buffer->getAudioFile()->getDuration();
	}
	ImGui::ProgressBar(m_FractionProgress, { 0.0f, 0.0f }, m_OpenFile->getPath().filename().string().c_str());
	
	ImGui::SameLine();
	
	if (ImGui::Button("Stop"))
	{
		if (m_Source->isPlaying())
		{
			m_Source->stop();
		}
	}
}
