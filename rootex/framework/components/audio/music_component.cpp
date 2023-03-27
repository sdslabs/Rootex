#include "music_component.h"
#include "audio_bus.h"
#include <string>

Map<ALuint, String> MusicComponent::music_comp_audio_bus = Map<ALuint, String>();
Map<ALuint, ALfloat> MusicComponent::music_comp_volume = Map<ALuint, ALfloat>();

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
	volume_comp = data.value("volume", 1.0f);
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

void MusicComponent::to_json(JSON::json& j, const MusicComponent& a)
{
	// write the code
	j["audioBusName"] = JSON::json::array();
	auto itr = music_comp_audio_bus.begin();
	while (itr != music_comp_audio_bus.end())
	{
		j["audioBusName"].push_back(itr->second);
		itr++;
	}
	j["ComponentPointer"] = JSON::json::array();
	itr = music_comp_audio_bus.begin();
	while (itr != music_comp_audio_bus.end())
	{
		auto val = itr->first;
		j["ComponentPointer"].emplace_back(itr->first);
		itr++;
	}
	j["ComponentVolume"] = JSON::json::array();
	auto itr2 = music_comp_volume.begin();
	while (itr2 != music_comp_volume.end())
	{
		j["ComponentPointer"].push_back(itr2->second);
		itr2++;
	}
}

void MusicComponent::from_json(const JSON::json& j, MusicComponent& a)
{
	Vector<String> temp;
	Vector<float> temp2;
	for (auto& name : j.value("audioBusName", Vector<String>()))
	{
		temp.push_back(name);
	}
	for (auto& vol : j.value("ComponentVolume", Vector<float>()))
	{
		temp2.push_back(vol);
	}
	int i = 0;
	for (auto& bus : j.value("ComponentPointer", Vector<ALuint>()))
	{
		MusicComponent::music_comp_audio_bus.insert({ bus, temp[i] });
		MusicComponent::music_comp_volume.insert({ bus, temp2[i] });
		i++;
	}
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

// void MusicComponent::updateVolume(float ratio)
// {
// 	getVolume(ratio);
// }

void MusicComponent::draw()
{
	AudioComponent::getAudioSource()->setVolume(volume_comp);
	ImGui::BeginGroup();
	ImGui::Text("%s", m_AudioFile->getPath().generic_string().c_str());
	ImGui::SameLine();
	if (ImGui::Button("Audio File"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_AudioFile->getPath().generic_string(), (int)m_AudioFile->getType() });
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Select Music"))
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
	if (ImGui::ListBoxHeader("Choose an audio bus"))
	{
		bool bl = false;
		auto component_pointer = m_StreamingAudioSource.get()->getSourceID();
		for (int j = 0; j < AudioBusCollection::all_buses.size(); j++)
		{
			if (ImGui::RadioButton(AudioBusCollection::all_buses[j].c_str(), bl))
			{
				music_comp_audio_bus.insert({component_pointer, AudioBusCollection::all_buses[j]});
				music_comp_volume.insert({ component_pointer, volume_comp });
			}
		}
		ImGui::ListBoxFooter();
	}
	AudioComponent::draw();
}
