#include "music_component.h"

DEFINE_COMPONENT(MusicComponent);

MusicComponent::MusicComponent(Entity& owner, const JSON::json& data)
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

	return AudioComponent::setupData();
}

JSON::json MusicComponent::getJSON() const
{
	JSON::json& j = AudioComponent::getJSON();

	j["audio"] = m_AudioFile->getPath().string();
	j["playOnStart"] = m_IsPlayOnStart;

	return j;
}

void MusicComponent::setAudioFile(Ref<AudioResourceFile> audioFile)
{
	m_AudioFile = audioFile;
	setupData();
}

void MusicComponent::draw()
{
	ImGui::Text("%s", m_AudioFile->getPath().generic_string().c_str());
	ImGui::SameLine();
	if (ImGui::Button("Audio File"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_AudioFile->getPath().generic_string(), (int)m_AudioFile->getType() });
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Select Music"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Audio), "game/assets/"))
		{
			setAudioFile(ResourceLoader::CreateAudioResourceFile(*result));
		}
	}

	AudioComponent::draw();
}
