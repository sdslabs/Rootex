#include "audio_system.h"

#include <chrono>
#include <thread>

#include "framework/ecs_factory.h"
#include "components/audio/music_component.h"
#include "components/audio/short_music_component.h"
#include "components/physics/rigid_body_component.h"
#include "core/audio/audio_source.h"
#include "core/audio/static_audio_buffer.h"
#include "core/audio/streaming_audio_buffer.h"
#include "scene_loader.h"

String AudioSystem::GetALErrorString(int errID)
{
	switch (errID)
	{
	case AL_NO_ERROR:
		return "OK";
	case AL_INVALID_NAME:
		return "Invalid Name";
	case AL_INVALID_ENUM:
		return "Invalid Enum";
	case AL_INVALID_VALUE:
		return "Invalid Value";
	case AL_INVALID_OPERATION:
		return "Invalid Operation";
	case AL_OUT_OF_MEMORY:
		return "Out of Memory";
	}
	return "Unknown Error: " + std::to_string(errID);
}

String AudioSystem::GetALCErrorString(int errID)
{
	switch (errID)
	{
	case ALC_NO_ERROR:
		return "OK";
	case ALC_INVALID_DEVICE:
		return "Invalid Device";
	case ALC_INVALID_CONTEXT:
		return "Invalid Context";
	case ALC_INVALID_ENUM:
		return "Invalid Enum";
	case ALC_INVALID_VALUE:
		return "Invalid Value";
	case ALC_OUT_OF_MEMORY:
		return "Out of Memory";
	}
	return "Unknown Error: " + std::to_string(errID);
}

void AudioSystem::CheckALError(const char* msg, const char* fname, int line)
{
	ALenum err = alGetError();
	if (err != AL_NO_ERROR)
	{
		ERR("OpenAL error (" + std::to_string(err) + "): " + GetALErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
	}
}

void AudioSystem::CheckALCError(const char* msg, const char* fname, int line)
{
	ALCenum err = alcGetError(GetSingleton()->m_Device);
	if (err != ALC_NO_ERROR)
	{
		ERR("OpenALC error (" + std::to_string(err) + "): " + GetALCErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
	}
}

void AudioSystem::CheckALUTError(const char* msg, const char* fname, int line)
{
	ALenum err = alutGetError();
	if (err != ALUT_ERROR_NO_ERROR)
	{
		ERR("ALUT error (" + std::to_string(err) + "): " + alutGetErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
	}
}

bool AudioSystem::initialize(const JSON::json& systemData)
{
	if (!alutInit(NULL, NULL))
	{
		ERR("AudioSystem: AL, ALC, ALUT failed to initialize");
		return false;
	}
	return true;
}

void AudioSystem::begin()
{
	for (auto& mc : ECSFactory::GetAllMusicComponent())
	{
		if (mc.isPlayOnStart())
		{
			mc.getAudioSource()->play();
		}
	}
	for (auto& smc : ECSFactory::GetAllShortMusicComponent())
	{
		if (smc.isPlayOnStart())
		{
			smc.getAudioSource()->play();
		}
	}
}

void AudioSystem::update(float deltaMilliseconds)
{
	ZoneScoped;

	for (auto& mc : ECSFactory::GetAllMusicComponent())
	{
		if (!(mc.getOwner().getScene()->getIsScenePaused() && m_IsSystemPaused))
		{
			mc.getAudioSource()->queueNewBuffers();
			mc.update();
		}
	}
	for (auto& smc : ECSFactory::GetAllShortMusicComponent())
	{
		if (!(smc.getOwner().getScene()->getIsScenePaused() && m_IsSystemPaused))
		{
			smc.update();
		}
	}

	if (m_Listener)
	{
		m_Listener->update();
	}
}

AudioSystem* AudioSystem::GetSingleton()
{
	static AudioSystem singleton;
	return &singleton;
}

void AudioSystem::end()
{
	for (auto& mc : ECSFactory::GetAllMusicComponent())
	{
		mc.getAudioSource()->stop();
	}
	for (auto& smc : ECSFactory::GetAllShortMusicComponent())
	{
		smc.getAudioSource()->stop();
	}
}
void AudioSystem::addNewBus()
{
	// adds a new bus to the vector and the tree
}

void AudioSystem::removeBus(AudioBus* bus)
{
	// removes an audio bus from the vector and the tree
}

AudioBus::AudioBus()
    : m_Volume(100.0f)
    , m_BusName("Bus " + AudioSystem::GetSingleton()->getAudioBuses().size()) // naming logic to be seen
    , m_Parent(nullptr) // if not the first bus, then we need to see how to assign this to the master bus in the starting
    , m_IsMaster(false)
{
	m_AudioComponents.clear();
	m_Children.clear();
}

void AudioBus::addAudioComponent(Ref<AudioComponent> cp)
{
	m_AudioComponents.push_back(cp);
}

void AudioSystem::draw()
{
	System::draw();
	for (int i = 0; i < m_Buses.size(); i++)
	{
		AudioBus* bus = m_Buses[i];
		ImGui::Text(bus->getBusName().c_str());

		if (ImGui::Button(ICON_ROOTEX_MINUS "##Remove Bus"))
		{
			removeBus(bus);
		}

		// volume of the bus
		ImGui::DragFloat("Volume", &bus->getBusVolume(), 1.0f, 0.0f, 100.0f);

		// selecting the parent
		if (ImGui::BeginCombo("Parent", m_Buses[0]->getBusName().c_str()))
		{
			for (int j = 0; j < i; j++)
			{
				AudioBus* cp = m_Buses[j];
				if (ImGui::Selectable(cp->getBusName().c_str()))
				{
					bus->setParent(cp);
				}
			}
			ImGui::EndCombo();
		}
	}

	if (ImGui::Button(ICON_ROOTEX_PLUS "##Add Bus"))
	{
		addNewBus();
	}

	ImGui::Text("AudioMixerSystem");
}

void AudioSystem::setListener(AudioListenerComponent* listenerComponent)
{
	m_Listener = listenerComponent;
}

void AudioSystem::setConfig(const SceneSettings& sceneSettings)
{
	Scene* listenerScene = SceneLoader::GetSingleton()->getRootScene()->findScene(sceneSettings.listener);
	if (!listenerScene)
	{
		ERR("Listener scene not found with ID " + std::to_string(sceneSettings.listener));
		restoreListener();
		return;
	}

	AudioListenerComponent* listen = listenerScene->getEntity().getComponent<AudioListenerComponent>();
	if (!listen)
	{
		ERR("AudioListenerComponent not found on entity " + listenerScene->getFullName());
		restoreListener();
		return;
	}

	setListener(listen);
}

void AudioSystem::restoreListener()
{
	if (SceneLoader::GetSingleton()->getRootScene())
	{
		setListener(SceneLoader::GetSingleton()->getRootScene()->getEntity().getComponent<AudioListenerComponent>());
	}
}

void AudioSystem::shutDown()
{
	alutExit();
}

AudioSystem::AudioSystem()
    : System("AudioSystem", UpdateOrder::Async, true)
{
	m_Buses.clear();
	m_Buses.push_back(nullptr); // add the master audio bus here => new AudioBus(is_Master = true)
}

void AudioBus::onVolumeChange(float delta)
{
	for (auto& ac : m_AudioComponents)
	{
		ac->changeVolume(delta);
		// the fraction of change is to be decided
	}

	for (auto& child : m_Children)
	{
		child->onVolumeChange(delta);
	}
}

void AudioBus::setParent(AudioBus* parent)
{
	m_Parent = parent;
}
