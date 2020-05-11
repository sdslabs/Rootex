#include "cpu_particles_visual_component.h"

#include "random.h"
#include "resource_loader.h"
#include "systems/render_system.h"
#include "timer.h"

Component* CPUParticlesVisualComponent::Create(const JSON::json& componentData)
{
	ParticleTemplate particalTemplate {
		{ 
			componentData["velocity"]["x"],
		    componentData["velocity"]["y"],
		    componentData["velocity"]["z"] 
		},
		{
			componentData["angularVelocity"]["x"], 
			componentData["angularVelocity"]["y"], 
			componentData["angularVelocity"]["z"], 
			componentData["angularVelocity"]["w"] 
		},
		{
			componentData["colorBegin"]["r"], 
			componentData["colorBegin"]["g"], 
			componentData["colorBegin"]["b"], 
			componentData["colorBegin"]["a"] 
		},
		{
			componentData["colorEnd"]["r"], 
			componentData["colorEnd"]["g"], 
			componentData["colorEnd"]["b"], 
			componentData["colorEnd"]["a"] 
		},
		componentData["velocityVariation"],
		componentData["sizeBegin"],
		componentData["sizeEnd"],
		componentData["sizeVariation"],
		componentData["lifeTime"]
	};

	CPUParticlesVisualComponent* particles = new CPUParticlesVisualComponent(componentData["poolSize"], componentData["resFile"], particalTemplate, MaterialLibrary::GetMaterial((String)componentData["material"]), componentData["isVisible"]);
	return particles;
}

Component* CPUParticlesVisualComponent::CreateDefault()
{
	CPUParticlesVisualComponent* particles = new CPUParticlesVisualComponent(1000, "rootex/assets/cube.obj", ParticleTemplate(), MaterialLibrary::GetDefaultMaterial(), true);
	return particles;
}

CPUParticlesVisualComponent::CPUParticlesVisualComponent(size_t poolSize, const String& particleModelPath, const ParticleTemplate& particleTemplate, Ref<Material> material, bool visibility)
    : ModelVisualComponent(RenderPassMain, material, ResourceLoader::CreateVisualModelResourceFile(particleModelPath), visibility)
    , m_ParticleTemplate(particleTemplate)
    , m_TransformComponent(nullptr)
{
	m_AllowedMaterials = { ColorMaterial::s_MaterialName };
	m_ParticlePool.resize(poolSize);
	m_PoolIndex = poolSize - 1;
	m_LastRenderTimePoint = std::chrono::high_resolution_clock::now();
	m_EmitRate = 0;
}

CPUParticlesVisualComponent::~CPUParticlesVisualComponent()
{
}

bool CPUParticlesVisualComponent::setup()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	if (!m_TransformComponent)
	{
		ERR("Transform Component not found on entity with CPU Particles Component: " + m_Owner->getFullName());
		return false;
	}
	return true;
}

bool CPUParticlesVisualComponent::preRender()
{
	ModelVisualComponent::preRender();

	int i = m_EmitRate;
	while (i >= 0)
	{
		emit(m_ParticleTemplate);
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
		// https://gamedev.stackexchange.com/a/157018/106158
		particle.m_Transform = Matrix::Transform(Matrix::CreateTranslation(particle.m_Velocity * delta) * particle.m_Transform, 0.5f * particle.m_AngularVelocity * delta);
	}

	return true;
}

void CPUParticlesVisualComponent::render(RenderPass renderPass)
{
	ColorMaterial* material = dynamic_cast<ColorMaterial*>(getMaterial());
	if (material == nullptr)
	{
		return;
	}
	if (renderPass & m_RenderPass)
	{
		for (auto& particle : m_ParticlePool)
		{
			if (!particle.m_IsActive)
			{
				continue;
			}

			float life = particle.m_LifeRemaining / particle.m_LifeTime;
			float size = particle.m_SizeBegin * (life) + particle.m_SizeEnd * (1.0f - life);

			Color color = Color::Lerp(particle.m_ColorEnd, particle.m_ColorBegin, life);

			RenderSystem::GetSingleton()->pushMatrixOverride(Matrix::CreateScale(size) * particle.m_Transform);
			material->setColor({ color });
			RenderSystem::GetSingleton()->getRenderer()->draw(m_VisualModelResourceFile->getVertexBuffer(), m_VisualModelResourceFile->getIndexBuffer(), getMaterial());
			RenderSystem::GetSingleton()->popMatrix();
		}
	}
}

void CPUParticlesVisualComponent::postRender()
{
	ModelVisualComponent::postRender();
	m_LastRenderTimePoint = std::chrono::high_resolution_clock::now();
}

void CPUParticlesVisualComponent::emit(const ParticleTemplate& particleTemplate)
{
	Particle& particle = m_ParticlePool[m_PoolIndex];

	particle.m_IsActive = true;
	particle.m_Transform = m_TransformComponent->getAbsoluteTransform();

	particle.m_Velocity = particleTemplate.m_Velocity;
	particle.m_Velocity.x += particleTemplate.m_VelocityVariation * (Random::Float() - 0.5f);
	particle.m_Velocity.y += particleTemplate.m_VelocityVariation * (Random::Float() - 0.5f);
	particle.m_Velocity.z += particleTemplate.m_VelocityVariation * (Random::Float() - 0.5f);

	particle.m_AngularVelocity = particleTemplate.m_AngularVelocity;

	particle.m_ColorBegin = particleTemplate.m_ColorBegin;
	particle.m_ColorEnd = particleTemplate.m_ColorEnd;

	particle.m_LifeTime = particleTemplate.m_LifeTime;
	particle.m_LifeRemaining = particleTemplate.m_LifeTime;
	particle.m_SizeBegin = particleTemplate.m_SizeBegin + particleTemplate.m_SizeVariation * (Random::Float() - 0.5f);
	particle.m_SizeEnd = particleTemplate.m_SizeEnd;

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}

JSON::json CPUParticlesVisualComponent::getJSON() const
{
	JSON::json& j = ModelVisualComponent::getJSON();

	j["poolSize"] = m_ParticlePool.size();
	j["velocity"]["x"] = m_ParticleTemplate.m_Velocity.x;
	j["velocity"]["y"] = m_ParticleTemplate.m_Velocity.y;
	j["velocity"]["z"] = m_ParticleTemplate.m_Velocity.z;
	j["velocityVariation"] = m_ParticleTemplate.m_VelocityVariation;
	j["angularVelocity"]["x"] = m_ParticleTemplate.m_AngularVelocity.x;
	j["angularVelocity"]["y"] = m_ParticleTemplate.m_AngularVelocity.y;
	j["angularVelocity"]["z"] = m_ParticleTemplate.m_AngularVelocity.z;
	j["angularVelocity"]["w"] = m_ParticleTemplate.m_AngularVelocity.w;
	j["colorBegin"]["r"] = m_ParticleTemplate.m_ColorBegin.x;
	j["colorBegin"]["g"] = m_ParticleTemplate.m_ColorBegin.y;
	j["colorBegin"]["b"] = m_ParticleTemplate.m_ColorBegin.z;
	j["colorBegin"]["a"] = m_ParticleTemplate.m_ColorBegin.w;
	j["colorEnd"]["r"] = m_ParticleTemplate.m_ColorEnd.x;
	j["colorEnd"]["g"] = m_ParticleTemplate.m_ColorEnd.y;
	j["colorEnd"]["b"] = m_ParticleTemplate.m_ColorEnd.z;
	j["colorEnd"]["a"] = m_ParticleTemplate.m_ColorEnd.w;
	j["sizeBegin"] = m_ParticleTemplate.m_SizeBegin;
	j["sizeEnd"] = m_ParticleTemplate.m_SizeEnd;
	j["sizeVariation"] = m_ParticleTemplate.m_SizeVariation;
	j["lifeTime"] = m_ParticleTemplate.m_LifeTime;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void CPUParticlesVisualComponent::draw()
{
	ModelVisualComponent::draw();

	ImGui::DragInt("Emit Rate", &m_EmitRate);
	ImGui::Separator();
	ImGui::Text("Particle", ImGuiTreeNodeFlags_CollapsingHeader);
	ImGui::DragFloat3("Velocity", &m_ParticleTemplate.m_Velocity.x);
	ImGui::DragFloat("Velocity Variation", &m_ParticleTemplate.m_VelocityVariation);
	ImGui::DragFloat4("##Angular Velocity", &m_ParticleTemplate.m_AngularVelocity.x);
	ImGui::SameLine();
	if (ImGui::Button("Angular Velocity"))
	{
		m_ParticleTemplate.m_AngularVelocity = { 0.0f, 0.0f, 0.0f, 0.0f };
	}
	ImGui::ColorEdit4("Color Begin", &m_ParticleTemplate.m_ColorBegin.x);
	ImGui::ColorEdit4("Color End", &m_ParticleTemplate.m_ColorEnd.x);
	ImGui::DragFloat("Size Begin", &m_ParticleTemplate.m_SizeBegin, 0.01f);
	ImGui::DragFloat("Size End", &m_ParticleTemplate.m_SizeEnd, 0.01f);
	ImGui::DragFloat("Size Variation", &m_ParticleTemplate.m_SizeVariation, 0.01f);
	ImGui::DragFloat("Lifetime", &m_ParticleTemplate.m_LifeTime, 0.01f);
}
#endif // ROOTEX_EDITOR
