#include "music_component.h"

Component* MusicComponent::Create(const JSON::json& componentData)
{
	MusicComponent* musicComponent = new MusicComponent(
	    ResourceLoader::CreateAudioResourceFile(componentData.value("audio", "rootex/assets/ball.wav")),
	    componentData.value("playOnStart", false),
	    componentData.value("isLooping", false),
	    componentData.value("isAttenuated", false),
	    (AudioSource::AttenuationModel)componentData.value("attenuationModel", (int)AudioSource::AttenuationModel::Linear),
	    (ALfloat)componentData.value("rollOffFactor", 1.0f),
	    (ALfloat)componentData.value("referenceDistance", 1.0f),
	    (ALfloat)componentData.value("maxDistance", 100.0f));
	return musicComponent;
}

MusicComponent::MusicComponent(AudioResourceFile* audioFile, bool playOnStart, bool isLooping, bool attenuation, AudioSource::AttenuationModel model,
    ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance)
    : AudioComponent(playOnStart, isLooping, attenuation, model, rolloffFactor, referenceDistance, maxDistance)
    , m_AudioFile(audioFile)
{
}

MusicComponent::~MusicComponent()
{
	m_StreamingAudioSource.reset();
}

bool MusicComponent::setupData()
{
	m_StreamingAudioSource.reset();
	m_StreamingAudioBuffer.reset(new StreamingAudioBuffer(m_AudioFile));
	m_StreamingAudioSource.reset(new StreamingAudioSource(m_StreamingAudioBuffer));

	setAudioSource(m_StreamingAudioSource.get());

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

JSON::json MusicComponent::getJSON() const
{
	JSON::json& j = AudioComponent::getJSON();

	j["audio"] = m_AudioFile->getPath().string();
	j["playOnStart"] = m_IsPlayOnStart;

	return j;
}

void MusicComponent::setAudioFile(AudioResourceFile* audioFile)
{
	m_AudioFile = audioFile;
	setupData();
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
#include "utility/imgui_helpers.h"
void MusicComponent::draw()
{
	ImGui::Text("%s", m_AudioFile->getPath().generic_string().c_str());
	ImGui::SameLine();
	if (ImGui::Button("Audio File"))
	{
		EventManager::GetSingleton()->call("OpenScript", "EditorOpenFile", m_AudioFile->getPath().string());
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Music"))
	{
		igfd::ImGuiFileDialog::Instance()->OpenModal("Music", "Choose Music", SupportedFiles.at(ResourceFile::Type::Audio), "game/assets/");
	}

	if (igfd::ImGuiFileDialog::Instance()->FileDialog("Music"))
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk)
		{
			String filePathName = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetFilePathName()).generic_string();
			setAudioFile(ResourceLoader::CreateAudioResourceFile(filePathName));
		}

		igfd::ImGuiFileDialog::Instance()->CloseDialog("Music");
	}

	AudioComponent::draw();
}
#endif // ROOTEX_EDITOR
