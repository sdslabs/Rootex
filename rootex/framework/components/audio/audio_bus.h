#pragma once
#include <vector>
#include <string>
#include "audio_listener_component.h"
#include "entity.h"
#include "imgui.h"
#include "systems/audio_system.h"
#include "core/audio/audio_source.h"
#include "component.h"


struct AudioBus
{
    std::string name;
    bool isMaster;
    float volume;
    std::vector<AudioBus *> children;
    AudioBus* parent;
};

class AudioSourceChild : public AudioSource
{
public:
	ALuint m_SourceID = AudioSource::m_SourceID;
	virtual float getDuration() const override;
	void setVolume(int volume)
	{
		AudioSource::setVolume(volume);
	}
};

class AudioBusCollection : public Component
{
	COMPONENT(AudioBusCollection, Category::Audio);
	int m_temp;

	public:
	AudioBusCollection(Entity& owner, const JSON::json& data);
	~AudioBusCollection() = default;
	static Vector<String> all_buses;
	static Vector<String> all_buses_parent;
	static Vector<float> all_buses_volume;
	int count = 0;
	void to_json(JSON::json& j, const AudioBusCollection& a);
	void from_json(const JSON::json& j, AudioBusCollection& a);
	void setAudioBusName(int ind, String name);
	void setAudioParentBusName(int ind, String name);
	void setAudioBusVolume(int ind, float volume);
    //AudioBus *getAudioBusByName(String name, AudioBus *master);
    //AudioBus *addAudioBus(String name,bool isMaster,float volume,AudioBus *parentBus);
    void volumeChange(String name, float val);
    //void setParentOfAudioBus(AudioBus *bus, AudioBus *parentBus);
    void removeAudioBus(String name);
	int getIndexByName(String name);
    void draw();
};
DECLARE_COMPONENT(AudioBusCollection);
