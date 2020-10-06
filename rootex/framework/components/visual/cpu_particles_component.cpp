#include "cpu_particles_component.h"

#include "random.h"
#include "resource_loader.h"
#include "systems/render_system.h"
#include "timer.h"

#include "renderer/material_library.h"

Component* CPUParticlesComponent::Create(const JSON::json& componentData)
{
	ParticleTemplate particalTemplate {
		{ 
			componentData["velocity"]["x"],
		    componentData["velocity"]["y"],
		    componentData["velocity"]["z"] 
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
		componentData["angularVelocityVariation"],
		componentData["sizeBegin"],
		componentData["sizeEnd"],
		componentData["sizeVariation"],
		componentData["lifeTime"]
	};

	CPUParticlesComponent* particles = new CPUParticlesComponent(
		componentData["poolSize"], 
		componentData["resFile"],
	    componentData["materialPath"],
		particalTemplate, 
		componentData["isVisible"],
	    componentData["renderPass"],
		(EmitMode)(int)componentData["emitMode"],
	    {
	        componentData["emitterDimensions"]["x"],
	        componentData["emitterDimensions"]["y"],
	        componentData["emitterDimensions"]["z"]
		});
	return particles;
}

Component* CPUParticlesComponent::CreateDefault()
{
	CPUParticlesComponent* particles = new CPUParticlesComponent(
		1000,
		"rootex/assets/cube.obj",
	    "rootex/assets/materials/default_particles.rmat",
		ParticleTemplate(),
		true,
		(unsigned int)RenderPass::Basic,
		EmitMode::Point,
	    { 1.0f, 1.0f, 1.0f });
	return particles;
}

CPUParticlesComponent::CPUParticlesComponent(size_t poolSize, const String& particleModelPath, const String& materialPath, const ParticleTemplate& particleTemplate, bool visibility, unsigned int renderPass, EmitMode emitMode, const Vector3& emitterDimensions)
    : ModelComponent(renderPass, ResourceLoader::CreateModelResourceFile(particleModelPath), {}, visibility, {})
    , m_BasicMaterial(std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial(materialPath)))
    , m_ParticleTemplate(particleTemplate)
    , m_TransformComponent(nullptr)
    , m_CurrentEmitMode(emitMode)
    , m_EmitterDimensions(emitterDimensions)
{
	m_AllowedMaterials = { BasicMaterial::s_MaterialName };
	expandPool(poolSize);
	m_EmitRate = 0;
}

bool CPUParticlesComponent::setup()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	if (!m_TransformComponent)
	{
		ERR("Transform Component not found on entity with CPU Particles Component: " + m_Owner->getFullName());
		return false;
	}
	return true;
}

bool CPUParticlesComponent::preRender(float deltaMilliseconds)
{
	ZoneNamedN(componentPreRender, "CPU Particles Pre-Render", true);

	ModelComponent::preRender(deltaMilliseconds);

	int i = m_EmitRate;
	while (i >= 0)
	{
		emit(m_ParticleTemplate);
		i--;
	}

	{
		ZoneNamedN(particleInterpolation, "Particle Interpolation", true);
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
			float delta = deltaMilliseconds * 1e-3;
			particle.m_LifeRemaining -= delta;
			particle.m_Transform = Matrix::CreateTranslation(particle.m_Velocity * delta) * Matrix::CreateFromYawPitchRoll(particle.m_AngularVelocity.x * delta, particle.m_AngularVelocity.y * delta, particle.m_AngularVelocity.z * delta) * particle.m_Transform;
		}
	}
	return true;
}

void CPUParticlesComponent::render()
{
	ZoneNamedN(componentRender, "CPU Particles Render", true);

	for (auto& particle : m_ParticlePool)
	{
		if (!particle.m_IsActive)
		{
			continue;
		}

		float life = particle.m_LifeRemaining / particle.m_LifeTime;
		float size = particle.m_SizeBegin * (life) + particle.m_SizeEnd * (1.0f - life);
		
		RenderSystem::GetSingleton()->pushMatrixOverride(Matrix::CreateScale(size) * particle.m_Transform);
		
		m_BasicMaterial->setColor(Color::Lerp(particle.m_ColorEnd, particle.m_ColorBegin, life));
		RenderSystem::GetSingleton()->getRenderer()->bind(m_BasicMaterial.get());
		
		for (auto& [material, meshes] : m_ModelResourceFile->getMeshes())
		{
			for (auto& mesh : meshes)
			{
				RenderSystem::GetSingleton()->getRenderer()->draw(mesh.m_VertexBuffer.get(), mesh.m_IndexBuffer.get());
			}
		}
		RenderSystem::GetSingleton()->popMatrix();
	}
}

void CPUParticlesComponent::emit(const ParticleTemplate& particleTemplate)
{
	ZoneNamedN(componentRender, "CPU Particles Emit", true);

	Particle& particle = m_ParticlePool[m_PoolIndex];

	particle.m_IsActive = true;

	static Matrix initialTransform;
	initialTransform = Matrix::Identity;
	switch (m_CurrentEmitMode)
	{
	case CPUParticlesComponent::EmitMode::Point:
		break;
	case CPUParticlesComponent::EmitMode::Square:
		initialTransform = Matrix::CreateTranslation({ Random::Float() * m_EmitterDimensions.x, 0, Random::Float() * m_EmitterDimensions.z });
		break;
	case CPUParticlesComponent::EmitMode::Cube:
		initialTransform = Matrix::CreateTranslation({ Random::Float() * m_EmitterDimensions.x, Random::Float() * m_EmitterDimensions.y, Random::Float() * m_EmitterDimensions.z });
		break;
	default:
		break;
	}

	particle.m_Transform = initialTransform * m_TransformComponent->getAbsoluteTransform();

	particle.m_Velocity = particleTemplate.m_Velocity;
	particle.m_Velocity.x += particleTemplate.m_VelocityVariation * (Random::Float() - 0.5f);
	particle.m_Velocity.y += particleTemplate.m_VelocityVariation * (Random::Float() - 0.5f);
	particle.m_Velocity.z += particleTemplate.m_VelocityVariation * (Random::Float() - 0.5f);

	particle.m_AngularVelocity = Vector3(Random::Float() - 0.5f, Random::Float() - 0.5f, Random::Float() - 0.5f) * particleTemplate.m_AngularVelocityVariation;
	particle.m_AngularVelocity.Normalize();

	particle.m_ColorBegin = particleTemplate.m_ColorBegin;
	particle.m_ColorEnd = particleTemplate.m_ColorEnd;

	particle.m_LifeTime = particleTemplate.m_LifeTime;
	particle.m_LifeRemaining = particleTemplate.m_LifeTime;
	particle.m_SizeBegin = particleTemplate.m_SizeBegin + particleTemplate.m_SizeVariation * (Random::Float() - 0.5f);
	particle.m_SizeEnd = particleTemplate.m_SizeEnd;

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}

void CPUParticlesComponent::expandPool(const size_t& poolSize)
{
	if (poolSize < 1)
	{
		return;
	}

	m_ParticlePool.resize(poolSize);
	m_PoolIndex = poolSize - 1;
}

JSON::json CPUParticlesComponent::getJSON() const
{
	JSON::json& j = ModelComponent::getJSON();

	j["materialPath"] = m_BasicMaterial->getFileName();

	j["poolSize"] = m_ParticlePool.size();
	j["velocity"]["x"] = m_ParticleTemplate.m_Velocity.x;
	j["velocity"]["y"] = m_ParticleTemplate.m_Velocity.y;
	j["velocity"]["z"] = m_ParticleTemplate.m_Velocity.z;
	j["velocityVariation"] = m_ParticleTemplate.m_VelocityVariation;
	j["angularVelocityVariation"] = m_ParticleTemplate.m_AngularVelocityVariation;
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
	j["emitMode"] = (int)m_CurrentEmitMode;
	j["emitterDimensions"]["x"] = m_EmitterDimensions.x;
	j["emitterDimensions"]["y"] = m_EmitterDimensions.y;
	j["emitterDimensions"]["z"] = m_EmitterDimensions.z;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void CPUParticlesComponent::draw()
{
	ImGui::Text("Model");
	ModelComponent::draw();

	ImGui::Separator();

	ImGui::Text("Particles Material");
	m_BasicMaterial->draw("0");

	ImGui::Separator();

	ImGui::Text("Emitter");
	static const char* emitModes[] = { 
		"Point",
		"Square",
		"Cube" 
	};
	ImGui::Combo("Emit Mode", (int*)&m_CurrentEmitMode, emitModes, 3);
	ImGui::DragFloat3("Emitter Dimensions", &m_EmitterDimensions.x);
	int poolSize = m_ParticlePool.size();
	if (ImGui::DragInt("Pool Size", &poolSize, 1.0f, 1, 100000)) 
	{
		expandPool(poolSize);
	}
	ImGui::DragInt("Emit Rate", &m_EmitRate);
	
	ImGui::Separator();
	
	ImGui::Text("Particle", ImGuiTreeNodeFlags_CollapsingHeader);
	ImGui::DragFloat3("Velocity", &m_ParticleTemplate.m_Velocity.x);
	ImGui::DragFloat("Velocity Variation", &m_ParticleTemplate.m_VelocityVariation);
	ImGui::DragFloat("Angular Velocity Variation", &m_ParticleTemplate.m_AngularVelocityVariation);
	ImGui::ColorEdit4("Color Begin", &m_ParticleTemplate.m_ColorBegin.x);
	ImGui::ColorEdit4("Color End", &m_ParticleTemplate.m_ColorEnd.x);
	ImGui::DragFloat("Size Begin", &m_ParticleTemplate.m_SizeBegin, 0.01f);
	ImGui::DragFloat("Size End", &m_ParticleTemplate.m_SizeEnd, 0.01f);
	ImGui::DragFloat("Size Variation", &m_ParticleTemplate.m_SizeVariation, 0.01f);
	ImGui::DragFloat("Lifetime", &m_ParticleTemplate.m_LifeTime, 0.01f);
}
#endif // ROOTEX_EDITOR
