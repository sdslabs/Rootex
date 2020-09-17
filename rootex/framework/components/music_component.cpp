#include "music_component.h"

Component* MusicComponent::Create(const JSON::json& componentData)
{
	MusicComponent* musicComponent = new MusicComponent(
	    ResourceLoader::CreateAudioResourceFile(componentData["audio"]),
	    (bool)componentData["playOnStart"],
	    (bool)componentData["isAttenuated"],
	    (AudioSource::AttenuationModel)componentData["attenuationModel"],
	    (ALfloat)componentData["rollOffFactor"],
	    (ALfloat)componentData["referenceDistance"],
	    (ALfloat)componentData["maxDistance"]);
	return musicComponent;
}

Component* MusicComponent::CreateDefault()
{
	MusicComponent* musicComponent
	    = new MusicComponent(
	        ResourceLoader::CreateAudioResourceFile("rootex/assets/ball.wav"),
	        false,
	        false,
	        AudioSource::AttenuationModel::Linear,
	        (ALfloat)1,
	        (ALfloat)1,
	        (ALfloat)100);
	return musicComponent;
}

MusicComponent::MusicComponent(AudioResourceFile* audioFile, bool playOnStart, bool attenuation, AudioSource::AttenuationModel model,
    ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance)
    : AudioComponent(playOnStart, attenuation, model, rolloffFactor, referenceDistance, maxDistance)
    , m_AudioFile(audioFile)
{
}

MusicComponent::~MusicComponent()
{
	m_StreamingAudioSource.reset();
}

bool MusicComponent::setup()
{
	m_StreamingAudioSource.reset();
	m_StreamingAudioBuffer.reset(new StreamingAudioBuffer(m_AudioFile));
	m_StreamingAudioSource.reset(new StreamingAudioSource(m_StreamingAudioBuffer));

	setAudioSource(m_StreamingAudioSource.get());

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
	setup();
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void MusicComponent::draw()
{
	ImGui::BeginGroup();

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
			if (IsFileSupported(payloadPath.extension().string(), ResourceFile::Type::Audio))
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
