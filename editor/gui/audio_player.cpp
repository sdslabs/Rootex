#include "audio_player.h"

#include "audio/audio_system.h"
#include "editor/editor.h"

#include "imgui.h"

void AudioPlayer::drawFileInfo()
{
	ImGui::Columns(2);

	ImGui::Text("Duration");
	ImGui::NextColumn();
	float totalDuration = m_OpenFile->getDuration();
	unsigned int minutes = totalDuration / MIN_TO_S;
	float seconds = totalDuration - minutes * MIN_TO_S;
	ImGui::Text("%d:%f", minutes, seconds);
	ImGui::NextColumn();

	ImGui::Text("Bit Depth");
	ImGui::NextColumn();
	ImGui::Text("%d bit", m_OpenFile->getBitDepth());
	ImGui::NextColumn();

	ImGui::Text("Channels");
	ImGui::NextColumn();
	ImGui::Text(std::to_string(m_OpenFile->getChannels()).c_str());
	ImGui::NextColumn();

	ImGui::Text("Format");
	ImGui::NextColumn();
	ImGui::Text(std::to_string(m_OpenFile->getFormat()).c_str());
	ImGui::NextColumn();

	ImGui::Text("Frequency");
	ImGui::NextColumn();
	ImGui::Text("%d Hz", (unsigned int)m_OpenFile->getFrequency());
	ImGui::NextColumn();

	ImGui::Columns(1);
}

AudioPlayer::AudioPlayer()
{
}

ResourceFile* AudioPlayer::load(const FilePath& filePath)
{
	m_FractionProgress = 0.0f;
	m_OpenFile = ResourceLoader::CreateAudioResourceFile(filePath.string());

	m_Buffer.reset(new StaticAudioBuffer(m_OpenFile));
	m_Source.reset(new StaticAudioSource(m_Buffer));

	PANIC(m_OpenFile == nullptr, "Audio file could not be loaded");

	return m_OpenFile;
}

void AudioPlayer::unload()
{
	if (m_Source && m_Buffer)
	{
		if (m_Source->isPlaying())
		{
			m_Source->stop();
		}
		m_Looping = false;
		m_Timer.reset();
		m_Source.reset();
		m_Buffer.reset();
	}
}

void AudioPlayer::draw()
{
	drawFileInfo();
	ImGui::NewLine();
	if (ImGui::Button("Play"))
	{
		if (!m_Source->isPlaying())
		{
			m_Source->play();
			m_Timer.reset();
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Loop", &m_Looping))
	{
		m_Source->setLooping(m_Looping);
	}
	ImGui::SameLine();

	m_FractionProgress = m_Source->getElapsedTimeS() / m_Buffer->getAudioFile()->getDuration();
	if (m_FractionProgress > 1.0f)
	{
		m_FractionProgress = 1.0f;
	}

	float timeSeconds = m_Source->getElapsedTimeS();
	int minutesUI = timeSeconds / MIN_TO_S;
	int secondsUI = timeSeconds - (minutesUI * MIN_TO_S);

	ImGui::ProgressBar(m_FractionProgress, { 0.0f, 0.0f },
	    (std::to_string(minutesUI) + ":" + ((secondsUI < 10) ? "0" + std::to_string(secondsUI) : std::to_string(secondsUI)) + " | " + m_OpenFile->getPath().filename().string()).c_str());

	ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		if (m_Source->isPlaying())
		{
			m_Source->stop();
			m_FractionProgress = 0.0f;
		}
	}

	if (m_OpenFile->isDirty())
	{
		ImGui::TextColored(Editor::GetSingleton()->getColors().m_Warning, "File may be changed on disk");
		ImGui::SameLine();
		if (ImGui::Button("Reload"))
		{
			m_OpenFile->reload();
		}
	}
}
