#include "short_music_component.h"

Ptr<Component> ShortMusicComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<ShortMusicComponent>(
	    ResourceLoader::CreateAudioResourceFile(componentData.value("audio", "rootex/assets/ball.wav")),
	    componentData.value("playOnStart", false),
	    componentData.value("isLooping", false),
	    componentData.value("isAttenuated", false),
	    (AudioSource::AttenuationModel)componentData.value("attenuationModel", (int)AudioSource::AttenuationModel::Linear),
	    (ALfloat)componentData.value("rollOffFactor", 1.0f),
	    (ALfloat)componentData.value("referenceDistance", 1.0f),
	    (ALfloat)componentData.value("maxDistance", 100.0f));
}

ShortMusicComponent::ShortMusicComponent(
    Ref<AudioResourceFile> audioFile,
    bool playOnStart,
    bool isLooping,
    bool attenuation,
    AudioSource::AttenuationModel model,
    ALfloat rolloffFactor,
    ALfloat referenceDistance,
    ALfloat maxDistance)
    : AudioComponent(playOnStart, isLooping, attenuation, model, rolloffFactor, referenceDistance, maxDistance)
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
	ImGui::Text("%s", m_AudioFile->getPath().generic_string().c_str());
	ImGui::SameLine();
	if (ImGui::Button("Audio File"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, m_AudioFile->getPath().string());
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Select Audio"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Audio), "game/assets/"))
		{
			setAudioFile(ResourceLoader::CreateAudioResourceFile(*result));
		}
	}

	AudioComponent::draw();
}
