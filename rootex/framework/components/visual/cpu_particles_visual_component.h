#pragma once

#include "model_visual_component.h"

struct ParticleTemplate
{
	Vector3 m_Velocity = { 1.0f, 0.0f, 0.0f };
	Quaternion m_AngularVelocity;
	Color m_ColorBegin = ColorPresets::Red;
	Color m_ColorEnd = ColorPresets::Blue;
	float m_VelocityVariation = 10.0f;
	float m_SizeBegin = 0.1f;
	float m_SizeEnd = 0.0f;
	float m_SizeVariation = 0.1f;
	float m_LifeTime = 1.0f;
};

class CPUParticlesVisualComponent : public ModelVisualComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();
	
	struct Particle
	{
		Vector3 m_Position;
		Vector3 m_Velocity;
		Quaternion m_AngularVelocity;
		Color m_ColorBegin;
		Color m_ColorEnd;
		Quaternion m_Rotation;
		float m_SizeBegin;
		float m_SizeEnd;
		float m_LifeTime;
		float m_LifeRemaining;
		bool m_IsActive = false;
	};

	ParticleTemplate m_ParticleTemplate;
	Vector<Particle> m_ParticlePool;
	size_t m_PoolIndex;
	int m_EmitRate;
	TransformComponent* m_TransformComponent;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_LastRenderTimePoint;

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::CPUParticlesVisualComponent;

	CPUParticlesVisualComponent(size_t poolSize, const String& particleModelPath, const ParticleTemplate& particleTemplate, bool visibility);
	CPUParticlesVisualComponent(VisualComponent&) = delete;
	virtual ~CPUParticlesVisualComponent();

	virtual bool setup() override;
	virtual bool preRender() override;
	virtual void render() override;
	virtual void postRender() override;

	void emit(const ParticleTemplate& particleTemplate);

	virtual String getName() const override { return "CPUParticlesVisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
