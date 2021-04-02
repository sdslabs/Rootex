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
	for (auto& c : ECSFactory::GetComponents<MusicComponent>())
	{
		MusicComponent* mc = (MusicComponent*)c;
		if (mc->isPlayOnStart())
		{
			mc->getAudioSource()->play();
		}
	}
	for (auto& c : ECSFactory::GetComponents<ShortMusicComponent>())
	{
		ShortMusicComponent* smc = (ShortMusicComponent*)c;
		if (smc->isPlayOnStart())
		{
			smc->getAudioSource()->play();
		}
	}
}

void AudioSystem::update(float deltaMilliseconds)
{
	ZoneScoped;

	for (auto& c : ECSFactory::GetComponents<MusicComponent>())
	{
		MusicComponent* mc = (MusicComponent*)c;
		mc->getAudioSource()->queueNewBuffers();
		mc->update();
	}
	for (auto& c : ECSFactory::GetComponents<ShortMusicComponent>())
	{
		ShortMusicComponent* smc = (ShortMusicComponent*)c;
		smc->update();
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
	for (auto& c : ECSFactory::GetComponents<MusicComponent>())
	{
		MusicComponent* mc = (MusicComponent*)c;
		mc->getAudioSource()->stop();
	}
	for (auto& c : ECSFactory::GetComponents<ShortMusicComponent>())
	{
		ShortMusicComponent* smc = (ShortMusicComponent*)c;
		smc->getAudioSource()->stop();
	}
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

	if (!listenerScene->getEntity())
	{
		ERR("Entity not found in listener scene " + listenerScene->getFullName());
		restoreListener();
		return;
	}

	AudioListenerComponent* listen = listenerScene->getEntity()->getComponent<AudioListenerComponent>();
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
		setListener(SceneLoader::GetSingleton()->getRootScene()->getEntity()->getComponent<AudioListenerComponent>());
	}
}

void AudioSystem::shutDown()
{
	alutExit();
}

AudioSystem::AudioSystem()
    : System("AudioSystem", UpdateOrder::Async, true)
    , m_Context(nullptr)
    , m_Device(nullptr)
    , m_Listener(nullptr)
{
}
