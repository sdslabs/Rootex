#pragma once

#include "common/common.h"

#include "components/space/transform_component.h"
#include "components/audio/audio_listener_component.h"
#include "components/audio/audio_component.h"

#include "al.h"
#include "alc.h"
#include "alut.h"

#include "system.h"

#ifndef ALUT_CHECK
#ifdef _DEBUG
#define ALUT_CHECK(alutFunction)                                        \
	do                                                                  \
	{                                                                   \
		alutFunction;                                                   \
		AudioSystem::CheckALUTError(#alutFunction, __FILE__, __LINE__); \
	} while (0)
#else
#define ALUT_CHECK(alutFunction) alutFunction
#endif
#endif

#ifndef AL_CHECK
#ifdef _DEBUG
#define AL_CHECK(alFunction)                                        \
	do                                                              \
	{                                                               \
		alFunction;                                                 \
		AudioSystem::CheckALError(#alFunction, __FILE__, __LINE__); \
	} while (0)
#else
#define AL_CHECK(alFunction) alFunction
#endif
#endif

class AudioBuffer;
class StaticAudioBuffer;
class StreamingAudioBuffer;

class AudioSource;
class StreamingAudioSource;

class ResourceFile;
class AudioComponent;

class AudioBus
{
private:
	String m_BusName;
	AudioBus* m_Parent = nullptr;
	Vector<Ref<AudioComponent>> m_AudioComponents;
	Vector<AudioBus*> m_Children;
	bool m_IsMaster; // really needed?
	float m_Volume;

	AudioBus();
	AudioBus(AudioBus&) = delete;
	~AudioBus() = default;

public:
	void addAudioComponent(Ref<AudioComponent> cp);
	void onVolumeChange(float delta);
	void setParent(AudioBus* parent);
	String getBusName() { return m_BusName; };
	float& getBusVolume() { return m_Volume; };
};


/// Audio System responsible for streaming and static audio
class AudioSystem : public System
{
	ALCdevice* m_Device = nullptr;
	ALCcontext* m_Context = nullptr;

	AudioListenerComponent* m_Listener = nullptr;
	AudioBus* m_RootAudioBus = nullptr;
	Vector<AudioBus*> m_Buses;

	AudioSystem();
	AudioSystem(AudioSystem&) = delete;
	virtual ~AudioSystem() = default;

public:
	static AudioSystem* GetSingleton();

	/// Returns error string corresponding to AL error codes.
	static String GetALErrorString(int errID);
	/// Returns error string corresponding to ALC error codes.
	static String GetALCErrorString(int errID);
	/// Wrapper over alGetError function.
	static void CheckALError(const char* msg, const char* fname, int line);
	/// Wrapper over alcGetError function.
	static void CheckALCError(const char* msg, const char* fname, int line);
	/// Wrapper over alutGetError function.
	static void CheckALUTError(const char* msg, const char* fname, int line);

	AudioListenerComponent* getListener() const { return m_Listener; }
	void setListener(AudioListenerComponent* listenerComponent);

	void restoreListener();

	bool initialize(const JSON::json& systemData) override;
	void setConfig(const SceneSettings& sceneSettings) override;
	void shutDown();

	void update(float deltaMilliseconds) override;
	void begin() override;
	void end() override;
	void draw() override;

	Vector<AudioBus*> getAudioBuses() const { return m_Buses; };
	void addNewBus();
	void removeBus(AudioBus* bus);
};
