#pragma once

#include "common/common.h"

class AudioBus
{
private:
	String m_BusName;
	AudioBus* m_Parent = nullptr;
	Vector<Ref<AudioComponent>> m_AudioComponents;
	Vector<AudioBus*> m_Children;
	bool m_IsMaster; // really needed?
	// TODO: change volume of components when m_Volume changes
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
