#include "cpu_particles_visual_component.h"

#include "systems/render_system.h"
#include "timer.h"

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
    : VisualComponent(RenderPassMain, Ref<CPUParticlesMaterial>(new CPUParticlesMaterial()), nullptr)
    , m_QuadVertices({ 
		{ { -0.5f, -0.5f, 0.0f } },   // 0
		{ { +0.5f, -0.5f, 0.0f } },   // 1
		{ { +0.5f, +0.5f, 0.0f } },   // 2
		{ { -0.5f, +0.5f, 0.0f } } }) // 3
    , m_QuadIndices({ 0, 1, 2, 2, 3, 0 })
{
	m_ParticlePool.resize(poolSize);
	m_PoolIndex = poolSize - 1;
	m_LastRenderTimePoint = std::chrono::high_resolution_clock::now();
	m_EmitRate = 0;
}

CPUParticlesVisualComponent::~CPUParticlesVisualComponent()
{
}

bool CPUParticlesVisualComponent::preRender()
{
	VisualComponent::preRender();
	
	int i = m_EmitRate;
	while (i >= 0)
	{
		emit(ParticleTemplate());
		i--;
	}

	for (auto& particle : m_ParticlePool)
	{
		if (particle.m_LifeRemaining <= 0.0f)
		{
			particle.m_IsActive = false;
			continue;
		}
		if (!particle.m_IsActive)
		{
			continue;
		}

		float delta = (std::chrono::high_resolution_clock::now() - m_LastRenderTimePoint).count() * (NS_TO_MS * MS_TO_S);
		particle.m_LifeRemaining -= delta;
		particle.m_Position += particle.m_Velocity * delta;
		particle.m_Rotation += 0.5f * particle.m_AngularVelocity * delta * particle.m_Rotation; // https://gamedev.stackexchange.com/a/157018/106158
	}

	return true;
}

void CPUParticlesVisualComponent::render()
{
	for (auto& particle : m_ParticlePool)
	{
		if (!particle.m_IsActive)
		{
			continue;
		}

		float life = particle.m_LifeRemaining / particle.m_LifeTime;
		float size = particle.m_SizeBegin * (life) + particle.m_SizeEnd * (1.0f - life);

		Color color = Color::Lerp(particle.m_ColorBegin, particle.m_ColorEnd, life);

		RenderSystem::GetSingleton()->pushMatrix(Matrix::CreateScale(size) * Matrix::CreateFromQuaternion(particle.m_Rotation) * Matrix::CreateTranslation(particle.m_Position) * Matrix::Identity);
		m_Attributes.getMaterial()->setShaderConstantBuffer(Shader::VertexConstantBufferType::Model, RenderSystem::GetSingleton()->getTopMatrix());
		m_Attributes.getMaterial()->setShaderConstantBuffer(PSSolidConstantBuffer({ color }));
		RenderSystem::GetSingleton()->getRenderer()->draw(&m_QuadVertices, &m_QuadIndices, m_Attributes.getMaterial());
		RenderSystem::GetSingleton()->popMatrix();
	}
}

void CPUParticlesVisualComponent::postRender()
{
	VisualComponent::postRender();
	m_LastRenderTimePoint = std::chrono::high_resolution_clock::now();
}

void CPUParticlesVisualComponent::emit(const ParticleTemplate& particleTemplate)
{
	Particle& particle = m_ParticlePool[m_PoolIndex];

	particle.m_IsActive = true;
	particle.m_Position = particleTemplate.Position;
	particle.m_Rotation = Quaternion::CreateFromAxisAngle({0.0f, 0.0f, 1.0f}, 1);

	particle.m_Velocity = particleTemplate.Velocity;
	particle.m_Velocity.x += particleTemplate.VelocityVariation.x * (rand() / INT_MAX) * 0.5f;
	particle.m_Velocity.y += particleTemplate.VelocityVariation.y * (rand() / INT_MAX) * 0.5f;

	particle.m_AngularVelocity = Quaternion::CreateFromAxisAngle({ 5.0f, 5.0f, 5.0f }, 10.0f);

	particle.m_ColorBegin = particleTemplate.ColorBegin;
	particle.m_ColorEnd = particleTemplate.ColorEnd;

	particle.m_LifeTime = particleTemplate.LifeTime;
	particle.m_LifeRemaining = particleTemplate.LifeTime;
	particle.m_SizeBegin = particleTemplate.SizeBegin + particleTemplate.SizeVariation * (rand() / INT_MAX) * 0.5f;
	particle.m_SizeEnd = particleTemplate.SizeEnd;

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
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
	ImGui::DragInt("Emit Rate", &m_EmitRate);
}
#endif // ROOTEX_EDITOR
