#include "music_component.h"

#include "core/resource_loader.h"
#include "core/event_manager.h"
#include "core/audio/streaming_audio_buffer.h"

Component* MusicComponent::Create(const JSON::json& componentData)
{
	MusicComponent* musicComponent = new MusicComponent(ResourceLoader::CreateAudioResourceFile(componentData["audio"]), (bool)componentData["playOnStart"]);
	return musicComponent;
}

Component* MusicComponent::CreateDefault()
{
	MusicComponent* musicComponent = new MusicComponent(ResourceLoader::CreateAudioResourceFile("rootex/assets/ball.wav"), false);
	return musicComponent;
}

MusicComponent::MusicComponent(AudioResourceFile* audioFile, bool playOnStart)
    : AudioComponent(playOnStart)
    , m_AudioFile(audioFile)
{
}

MusicComponent::~MusicComponent()
{
	m_StreamingAudioSource->stop();
}

bool MusicComponent::setup()
{
	m_StreamingAudioSource.reset();
	m_StreamingAudioBuffer.reset(new StreamingAudioBuffer(m_AudioFile));
	m_StreamingAudioSource.reset(new StreamingAudioSource(m_StreamingAudioBuffer));
	return true;
}

JSON::json MusicComponent::getJSON() const
{
	JSON::json j;

	j["audio"] = m_AudioFile->getPath().string();
	j["playOnStart"] = m_IsPlayOnStart;

	return j;
}

void MusicComponent::setAudioFile(AudioResourceFile* audioFile)
{
	m_AudioFile = audioFile;
	setup();
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void MusicComponent::draw()
{
	ImGui::BeginGroup();
	if (ImGui::BeginCombo("##Audio File", m_AudioFile->getPath().filename().string().c_str(), ImGuiComboFlags_HeightRegular))
	{
		for (auto&& file : ResourceLoader::GetFilesOfType(ResourceFile::Type::Wav))
		{
			if (ImGui::MenuItem(file->getPath().string().c_str(), ""))
			{
				setAudioFile((AudioResourceFile*)file);
			}
		}

		ImGui::Separator();

		static String inputPath = "Path";
		ImGui::InputText("##Path", &inputPath);
		ImGui::SameLine();
		if (ImGui::Button("Create Audio File"))
		{
			if (!ResourceLoader::CreateAudioResourceFile(inputPath))
			{
				WARN("Could not create Audio File");
			}
			else
			{
				inputPath = "";
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if (ImGui::Button("Audio File"))
	{
		EventManager::GetSingleton()->call("OpenScript", "EditorOpenFile", m_AudioFile->getPath());
	}
	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (payloadPath.extension() == ".wav")
			{
				setAudioFile(ResourceLoader::CreateAudioResourceFile(payloadPath.string()));
			}
			else
			{
				WARN("Cannot assign a non-wav file to Audio File");
			}
		}
		ImGui::EndDragDropTarget();
	}
}
#endif // ROOTEX_EDITOR
