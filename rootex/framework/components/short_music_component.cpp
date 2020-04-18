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

bool ShortMusicComponent::setup()
{
	m_StaticAudioSource.reset();
	m_StaticAudioBuffer.reset(new StaticAudioBuffer(m_AudioFile));
	m_StaticAudioSource.reset(new StaticAudioSource(m_StaticAudioBuffer));

	setAudioSource(m_StaticAudioSource.get());

	bool status = AudioComponent::setup();
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
	setup();
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ShortMusicComponent::draw()
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
		EventManager::GetSingleton()->call("OpenScript", "EditorOpenFile", m_AudioFile->getPath().string());
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

	AudioComponent::draw();
}
#endif // ROOTEX_EDITOR
