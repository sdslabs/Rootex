#include "cpu_particles_visual_component.h"

Component* CPUParticlesVisualComponent::Create(const JSON::json& componentData)
{
	CPUParticlesVisualComponent* particles = new CPUParticlesVisualComponent(componentData["poolSize"]);
	return particles;
}

Component* CPUParticlesVisualComponent::CreateDefault()
{
	CPUParticlesVisualComponent* particles = new CPUParticlesVisualComponent(1000);
	return particles;
}

CPUParticlesVisualComponent::CPUParticlesVisualComponent(size_t poolSize)
    : VisualComponent(RenderPassMain, Ref<CPUParticlesMaterial>(), nullptr)
{
	m_ParticlePool.resize(poolSize);
	m_PoolIndex = poolSize - 1;
	m_LastRenderTimePoint = std::chrono::high_resolution_clock::now();
}

CPUParticlesVisualComponent::~CPUParticlesVisualComponent()
{
}

bool CPUParticlesVisualComponent::preRender()
{
	for (auto&& particle : m_ParticlePool)
	{
		if (!particle.m_IsActive)
		{
			continue;
		}

		if (particle.m_LifeRemaining <= 0.0f)
		{
			particle.m_IsActive = false;
			continue;
		}

		float delta = (std::chrono::high_resolution_clock::now() - m_LastRenderTimePoint).count() * 1e-3;
		particle.m_LifeRemaining -= delta;
		particle.m_Position += particle.m_Velocity * delta;
		// https://gamedev.stackexchange.com/a/157018/106158
		particle.m_Rotation += 0.5f * particle.m_AngularVelocity * delta * particle.m_Rotation;
	}

	return true;
}

void CPUParticlesVisualComponent::render()
{
}

void CPUParticlesVisualComponent::postRender()
{
	m_LastRenderTimePoint = std::chrono::high_resolution_clock::now();
}

JSON::json CPUParticlesVisualComponent::getJSON() const
{
	JSON::json j;

	j["poolSize"] = m_ParticlePool.size();

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void CPUParticlesVisualComponent::draw()
{
}
#endif // ROOTEX_EDITOR
