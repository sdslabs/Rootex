#include "short_music_component.h"

Component* ShortMusicComponent::Create(const JSON::json& componentData)
{
	ShortMusicComponent* shortMusicComponent = new ShortMusicComponent(
	    ResourceLoader::CreateAudioResourceFile(componentData["audio"]),
	    (bool)componentData["playOnStart"],
	    (bool)componentData["isAttenuated"],
	    (AudioSource::AttenuationModel)componentData["attenuationModel"],
	    (ALfloat)componentData["rolloffFactor"],
	    (ALfloat)componentData["referenceDistance"],
	    (ALfloat)componentData["maxDistance"]);
	return shortMusicComponent;
}

Component* ShortMusicComponent::CreateDefault()
{
	ShortMusicComponent* shortMusicComponent = new ShortMusicComponent(
	    ResourceLoader::CreateAudioResourceFile("rootex/assets/ball.wav"),
	    false,
	    false,
	    AudioSource::AttenuationModel::Linear,
	    (ALfloat)1,
	    (ALfloat)1,
	    (ALfloat)100);
	return shortMusicComponent;
}

ShortMusicComponent::ShortMusicComponent(AudioResourceFile* audioFile, bool playOnStart, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance)
    : AudioComponent(playOnStart, attenuation, model, rolloffFactor, referenceDistance, maxDistance)
    , m_AudioFile(audioFile)
{
}

ShortMusicComponent::~ShortMusicComponent()
{
	m_StaticAudioSource.reset();
}

bool ShortMusicComponent::setupData()
{
	m_StaticAudioSource.reset();
	m_StaticAudioBuffer.reset(new StaticAudioBuffer(m_AudioFile));
	m_StaticAudioSource.reset(new StaticAudioSource(m_StaticAudioBuffer));

	setAudioSource(m_StaticAudioSource.get());

	bool status = AudioComponent::setupData();
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (m_TransformComponent == nullptr)
		{
			WARN("Entity without transform component!");
			status = false;
		}
	}
	return status;
}

JSON::json ShortMusicComponent::getJSON() const
{
	JSON::json& j = AudioComponent::getJSON();

	j["audio"] = m_AudioFile->getPath().string();
	j["playOnStart"] = m_IsPlayOnStart;
	return j;
}

void ShortMusicComponent::setAudioFile(AudioResourceFile* audioFile)
{
	m_AudioFile = audioFile;
	setupData();
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
#include "utility/imgui_helpers.h"
void ShortMusicComponent::draw()
{
	ImGui::Text("%s", m_AudioFile->getPath().generic_string().c_str());
	ImGui::SameLine();
	if (ImGui::Button("Audio File"))
	{
		EventManager::GetSingleton()->call("OpenScript", "EditorOpenFile", m_AudioFile->getPath().string());
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Short Music"))
	{
		igfd::ImGuiFileDialog::Instance()->OpenDialog("Short Music", "Choose Short Music", SupportedFiles.at(ResourceFile::Type::Audio), "game/assets/");
	}

	if (igfd::ImGuiFileDialog::Instance()->FileDialog("Short Music"))
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk)
		{
			String filePathName = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetFilePathName()).generic_string();
			setAudioFile(ResourceLoader::CreateAudioResourceFile(filePathName));
		}

		igfd::ImGuiFileDialog::Instance()->CloseDialog("Short Music");
	}

	AudioComponent::draw();
}
#endif // ROOTEX_EDITOR
