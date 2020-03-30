#pragma once

#include "visual_component.h"

class CPUParticlesVisualComponent : public VisualComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();
	
	struct Particle
	{
		Vector3 m_Position;
		Vector3 m_Velocity;
		Quaternion m_AngularVelocity;
		Color m_ColourBegin;
		Color m_ColourEnd;
		Quaternion m_Rotation;
		float m_SizeBegin;
		float m_SizeEnd;
		float m_Lifetime;
		float m_LifeRemaining;
		bool m_IsActive = false;
	};

	Vector<Particle> m_ParticlePool;
	size_t m_PoolIndex;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_LastRenderTimePoint;

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::CPUParticlesVisualComponent;

	CPUParticlesVisualComponent(size_t poolSize);
	CPUParticlesVisualComponent(VisualComponent&) = delete;
	virtual ~CPUParticlesVisualComponent();

	virtual bool preRender();
	virtual void render();
	virtual void postRender();

	virtual String getName() const override { return "CPUParticlesVisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
