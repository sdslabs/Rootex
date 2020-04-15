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

	bool status = AudioComponent::setup();
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (m_TransformComponent == nullptr)
		{
			WARN("Entity without transform component!");
			status = false;
		}
		MusicComponent::getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
		MusicComponent::getAudioSource()->setModel(m_AttenuationModel);
		MusicComponent::getAudioSource()->setRollOffFactor(m_RolloffFactor);
		MusicComponent::getAudioSource()->setReferenceDistance(m_ReferenceDistance);
		MusicComponent::getAudioSource()->setMaxDistance(m_MaxDistance);
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

	ImGui::Checkbox("Play on start", &m_IsPlayOnStart);

	//static const char* models[] { "Linear", "Inverse", "Exponential" };
	//static int current_model = 0;
	//static bool IsClamped = false;

	//if (m_IsAttenuated)
	//{
	//ImGui::Combo("Attenuation model ", &current_model, models, IM_ARRAYSIZE(models), 1);
	//ImGui::Checkbox("Turn on Clamping", &IsClamped);

	//ImGui::SliderFloat("Reference Distance", (ALfloat)m_referenceDistance, 0, 10000);
	//ImGui::SliderFloat("Rolloff Factor", (ALfloat)m_rolloffFactor,0,10000);
	//ImGui::SliderFloat("Max Distance", (ALfloat)m_maxDistance, 0, 10000);
	//}
	//ImGui::Checkbox("Enable Volume Attenuation", &m_IsAttenuated);
}
#endif // ROOTEX_EDITOR
