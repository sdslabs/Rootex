#pragma once

#include "system.h"

#include "Effekseer.h"
#include "EffekseerRendererDX11.h"
#include "EffekseerSoundAL.h"

class ParticleSystem : public System
{
	EffekseerRendererDX11::Renderer* m_Renderer = nullptr;
	Effekseer::Manager* m_Manager = nullptr;
	EffekseerSound::Sound* m_Sound = nullptr;

	float m_FrameProgression;
	float m_TargetUPS;

	ParticleSystem();
	ParticleSystem(const ParticleSystem&) = delete;
	~ParticleSystem();

public:
	static ParticleSystem* GetSingleton();

	bool initialize(const JSON::json& systemData) override;
	void begin() override;
	void update(float deltaMilliseconds) override;

	Effekseer::Handle play(Effekseer::Effect* effect, const Vector3& position, int startFrame);
	void stop(Effekseer::Handle handle);

	void setMatrix(Effekseer::Handle handle, const Matrix& mat);
	void setSpeed(Effekseer::Handle handle, const float& speed);
	void setTargetLocation(Effekseer::Handle handle, const Vector3& target);

	bool getPaused(Effekseer::Handle handle);

	Effekseer::Effect* loadEffect(const String& path);
};
