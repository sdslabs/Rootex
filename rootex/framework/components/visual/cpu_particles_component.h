#pragma once

#include "model_component.h"

struct ParticleTemplate
{
	Vector3 m_Velocity = { 1.0f, 0.0f, 0.0f };
	Color m_ColorBegin = ColorPresets::Red;
	Color m_ColorEnd = ColorPresets::Blue;
	float m_VelocityVariation = 10.0f;
	float m_SizeBegin = 0.1f;
	float m_SizeEnd = 0.0f;
	float m_SizeVariation = 0.1f;
	float m_LifeTime = 1.0f;
};

class CPUParticlesComponent : public ModelComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();
	
	struct Particle
	{
		Matrix m_Transform;
		Vector3 m_Velocity;
		Vector3 m_AngularVelocity;
		Color m_ColorBegin;
		Color m_ColorEnd;
		float m_SizeBegin;
		float m_SizeEnd;
		float m_LifeTime;
		float m_LifeRemaining;
		bool m_IsActive = false;
	};

	ParticleTemplate m_ParticleTemplate;
	Vector<Particle> m_ParticlePool;
	Ref<BasicMaterial> m_BasicMaterial;
	size_t m_PoolIndex;
	int m_EmitRate;
	TransformComponent* m_TransformComponent;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_LastRenderTimePoint;

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::CPUParticlesComponent;

	CPUParticlesComponent(size_t poolSize, const String& particleModelPath, const String& materialPath, const ParticleTemplate& particleTemplate, bool visibility, unsigned int renderPass);
	CPUParticlesComponent(CPUParticlesComponent&) = delete;
	virtual ~CPUParticlesComponent() = default;

	virtual bool setup() override;
	virtual bool preRender() override;
	virtual void render() override;
	virtual void postRender() override;

	void emit(const ParticleTemplate& particleTemplate);
	void expandPool(const size_t& poolSize);

	virtual String getName() const override { return "CPUParticlesComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
