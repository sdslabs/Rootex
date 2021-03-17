#include "music_component.h"

Ptr<Component> MusicComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<MusicComponent>(
	    ResourceLoader::CreateAudioResourceFile(componentData.value("audio", "rootex/assets/ball.wav")),
	    componentData.value("playOnStart", false),
	    componentData.value("isLooping", false),
	    componentData.value("isAttenuated", false),
	    (AudioSource::AttenuationModel)componentData.value("attenuationModel", (int)AudioSource::AttenuationModel::Linear),
	    (ALfloat)componentData.value("rollOffFactor", 1.0f),
	    (ALfloat)componentData.value("referenceDistance", 1.0f),
	    (ALfloat)componentData.value("maxDistance", 100.0f));
}

MusicComponent::MusicComponent(
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
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, m_AudioFile->getPath().string());
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Music"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Audio), "game/assets/"))
		{
			setAudioFile(ResourceLoader::CreateAudioResourceFile(*result));
		}
	}

	AudioComponent::draw();
}
