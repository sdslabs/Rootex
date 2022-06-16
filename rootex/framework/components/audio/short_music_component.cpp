#include "short_music_component.h"

DEFINE_COMPONENT(ShortMusicComponent);

ShortMusicComponent::ShortMusicComponent(Entity& owner, const JSON::json& data)
    : AudioComponent(
        owner,
        data.value("playOnStart", false),
        data.value("volume", 1.0f),
        data.value("isLooping", false),
        data.value("isAttenuated", false),
        (AudioSource::AttenuationModel)data.value("attenuationModel", (int)AudioSource::AttenuationModel::Linear),
        (ALfloat)data.value("rollOffFactor", 1.0f),
        (ALfloat)data.value("referenceDistance", 1.0f),
        (ALfloat)data.value("maxDistance", 100.0f))
    , m_AudioFile(ResourceLoader::CreateAudioResourceFile(data.value("audio", "rootex/assets/ball.wav")))
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

	return AudioComponent::setupData();
}

JSON::json ShortMusicComponent::getJSON() const
{
	JSON::json& j = AudioComponent::getJSON();

	j["audio"] = m_AudioFile->getPath().generic_string();
	j["playOnStart"] = m_IsPlayOnStart;

	return j;
}

void ShortMusicComponent::setAudioFile(Ref<AudioResourceFile> audioFile)
{
	m_AudioFile = audioFile;
	setupData();
}

void ShortMusicComponent::draw()
{
	ImGui::BeginGroup();
	ImGui::Text("%s", m_AudioFile->getPath().generic_string().c_str());
	ImGui::SameLine();
	if (ImGui::Button("Audio File"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_AudioFile->getPath().generic_string(), (int)m_AudioFile->getType() });
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Select Audio"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Audio), "game/assets/"))
		{
			setAudioFile(ResourceLoader::CreateAudioResourceFile(*result));
		}
	}
	ImGui::EndGroup();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AUDIO_PAYLOAD"))
		{
			const char* path = (const char*)payload->Data;
			setAudioFile(ResourceLoader::CreateAudioResourceFile(path));
		}
	}
	AudioComponent::draw();
}
