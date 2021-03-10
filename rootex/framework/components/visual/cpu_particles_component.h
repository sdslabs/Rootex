#pragma once

#include "core/renderer/materials/particles_material.h"
#include "renderer/vertex_data.h"
#include "model_component.h"

#define MAX_PARTICLES 5000

struct ParticleTemplate
{
	Vector3 velocity = { 1.0f, 0.0f, 0.0f };
	Color colorBegin = ColorPresets::Red;
	Color colorEnd = ColorPresets::Blue;
	float velocityVariation = 10.0f;
	float rotationVariation = DirectX::XM_PI;
	float angularVelocityVariation = 0.5f;
	float sizeBegin = 0.1f;
	float sizeEnd = 0.0f;
	float sizeVariation = 0.1f;
	float lifeTime = 1.0f;
};

class CPUParticlesComponent : public ModelComponent
{
	static Component* Create(const JSON::json& componentData);

	Vector<InstanceData> m_InstanceBufferData;
	Vector<InstanceData> m_InstanceBufferLiveData;
	int m_LiveParticlesCount;
	Ptr<VertexBuffer> m_InstanceBuffer;

	struct Particle
	{
		float sizeBegin;
		float sizeEnd;
		float lifeTime;
		float lifeRemaining;
		Color colorBegin;
		Color colorEnd;
		Vector3 velocity;
		Vector3 angularVelocity;

		// Not for use outside
		Vector3 position;
		Quaternion rotation;
		Vector3 scale;
	};

	ParticleTemplate m_ParticleTemplate;
	Vector<Particle> m_ParticlePool;
	Ref<ParticlesMaterial> m_ParticlesMaterial;
	size_t m_PoolIndex;
	float m_EmitRate;

	enum class EmitMode : int
	{
		Point = 0,
		Square = 1,
		Cube = 2,
		Sphere = 3,
		End
	};

	EmitMode m_CurrentEmitMode;
	Vector3 m_EmitterDimensions;

	float m_EmitCount = 0;

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::CPUParticlesComponent;

	CPUParticlesComponent(
	    size_t poolSize,
	    const String& particleModelPath,
	    const String& materialPath,
	    const ParticleTemplate& particleTemplate,
	    EmitMode emitMode,
	    int emitRate,
	    const Vector3& emitterDimensions,
	    bool visibility,
	    unsigned int renderPass);
	CPUParticlesComponent(CPUParticlesComponent&) = delete;
	virtual ~CPUParticlesComponent() = default;

	virtual bool setupData() override;
	virtual bool preRender(float deltaMilliseconds) override;
	virtual void render(float viewDistance) override;

	void setMaterial(Ref<ParticlesMaterial> particlesMaterial);
	void emit(const ParticleTemplate& particleTemplate);
	void expandPool(const size_t& poolSize);

	virtual const char* getName() const override { return "CPUParticlesComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

	void draw() override;
};
