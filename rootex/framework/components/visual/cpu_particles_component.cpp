#include "cpu_particles_component.h"

#include "random.h"
#include "resource_loader.h"
#include "systems/render_system.h"
#include "timer.h"

#include "renderer/material_library.h"

void to_json(JSON::json& j, const ParticleTemplate p)
{
	j["velocity"] = p.velocity;
	j["colorBegin"] = p.colorBegin;
	j["colorEnd"] = p.colorEnd;
	j["velocityVariation"] = p.velocityVariation;
	j["angularVelocityVariation"] = p.angularVelocityVariation;
	j["sizeBegin"] = p.sizeBegin;
	j["sizeEnd"] = p.sizeBegin;
	j["sizeVariation"] = p.sizeVariation;
	j["lifeTime"] = p.lifeTime;
}

void from_json(const JSON::json& j, ParticleTemplate& p)
{
	p.velocity = j.at("velocity");
	p.colorBegin = j.at("colorBegin");
	p.colorEnd = j.at("colorEnd");
	p.velocityVariation = j.at("velocityVariation");
	p.angularVelocityVariation = j.at("angularVelocityVariation");
	p.sizeBegin = j.at("sizeBegin");
	p.sizeBegin = j.at("sizeEnd");
	p.sizeVariation = j.at("sizeVariation");
	p.lifeTime = j.at("lifeTime");
}

Component* CPUParticlesComponent::Create(const JSON::json& componentData)
{
	CPUParticlesComponent* particles = new CPUParticlesComponent(
	    componentData.value("poolSize", 1000),
		componentData.value("resFile", "rootex/assets/cube.obj"),
	    componentData.value("materialPath", "rootex/assets/materials/default_particles.rmat"),
	    componentData.value("particleTemplate", ParticleTemplate()), 
		componentData.value("isVisible", true),
	    componentData.value("renderPass", (unsigned int)RenderPass::Basic),
	    (EmitMode)componentData.value("emitMode", (int)EmitMode::Point),
	    componentData.value("emitterDimensions", Vector3 { 1.0f, 1.0f, 1.0f }));
	return particles;
}

CPUParticlesComponent::CPUParticlesComponent(size_t poolSize, const String& particleModelPath, const String& materialPath, const ParticleTemplate& particleTemplate, bool visibility, unsigned int renderPass, EmitMode emitMode, const Vector3& emitterDimensions)
    : ModelComponent(renderPass, ResourceLoader::CreateModelResourceFile(particleModelPath), {}, visibility, {})
    , m_BasicMaterial(std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial(materialPath)))
    , m_ParticleTemplate(particleTemplate)
    , m_CurrentEmitMode(emitMode)
    , m_EmitterDimensions(emitterDimensions)
{
	m_AllowedMaterials = { BasicMaterial::s_MaterialName };
	expandPool(poolSize);
	m_EmitRate = 0;
}

bool CPUParticlesComponent::setupData()
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
	ZoneScoped;

	ModelComponent::preRender(deltaMilliseconds);

	int i = m_EmitRate;
	while (i >= 0)
	{
		emit(m_ParticleTemplate);
		i--;
	}

	{
		ZoneNamedN(particleInterpolation, "Particle Interpolation", true);
		
		float delta = deltaMilliseconds * 1e-3;
		for (auto& particle : m_ParticlePool)
		{

			if (particle.lifeRemaining <= 0.0f)
			{
				particle.isActive = false;
				continue;
			}
			if (!particle.isActive)
			{
				continue;
			}
			particle.lifeRemaining -= delta;
			particle.transform = Matrix::CreateTranslation(particle.velocity * delta) * Matrix::CreateFromYawPitchRoll(particle.angularVelocity.x * delta, particle.angularVelocity.y * delta, particle.angularVelocity.z * delta) * particle.transform;
		}
	}
	return true;
}

void CPUParticlesComponent::render()
{
	ZoneScoped;

	for (auto& particle : m_ParticlePool)
	{
		if (!particle.isActive)
		{
			continue;
		}

		float life = particle.lifeRemaining / particle.lifeTime;
		float size = particle.sizeBegin * (life) + particle.sizeEnd * (1.0f - life);
		
		RenderSystem::GetSingleton()->pushMatrixOverride(Matrix::CreateScale(size) * particle.transform);
		
		m_BasicMaterial->setColor(Color::Lerp(particle.colorEnd, particle.colorBegin, life));
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

void CPUParticlesComponent::setMaterial(Ref<BasicMaterial> particlesMaterial)
{
	if (!particlesMaterial)
	{
		WARN("Particles was tried to set to nullptr. Reverted to previously set material");
		return;
	}

	m_BasicMaterial = particlesMaterial;
}

void CPUParticlesComponent::emit(const ParticleTemplate& particleTemplate)
{
	ZoneScoped;

	Particle& particle = m_ParticlePool[m_PoolIndex];

	particle.isActive = true;

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

	particle.transform = initialTransform * m_TransformComponent->getAbsoluteTransform();

	particle.velocity = particleTemplate.velocity;
	particle.velocity.x += particleTemplate.velocityVariation * (Random::Float() - 0.5f);
	particle.velocity.y += particleTemplate.velocityVariation * (Random::Float() - 0.5f);
	particle.velocity.z += particleTemplate.velocityVariation * (Random::Float() - 0.5f);

	particle.angularVelocity = Vector3(Random::Float() - 0.5f, Random::Float() - 0.5f, Random::Float() - 0.5f) * particleTemplate.angularVelocityVariation;
	particle.angularVelocity.Normalize();

	particle.colorBegin = particleTemplate.colorBegin;
	particle.colorEnd = particleTemplate.colorEnd;

	particle.lifeTime = particleTemplate.lifeTime;
	particle.lifeRemaining = particleTemplate.lifeTime;
	particle.sizeBegin = particleTemplate.sizeBegin + particleTemplate.sizeVariation * (Random::Float() - 0.5f);
	particle.sizeEnd = particleTemplate.sizeEnd;

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
	j["particleTemplate"] = m_ParticleTemplate;
	j["emitMode"] = (int)m_CurrentEmitMode;
	j["emitterDimensions"]["x"] = m_EmitterDimensions.x;
	j["emitterDimensions"]["y"] = m_EmitterDimensions.y;
	j["emitterDimensions"]["z"] = m_EmitterDimensions.z;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "utility/imgui_helpers.h"
void CPUParticlesComponent::draw()
{
	ImGui::Text("Model");
	ModelComponent::draw();

	ImGui::Separator();

	ImGui::Text("%s", "Particles Material");
	ImGui::Image(m_BasicMaterial->getPreview(), { 50, 50 });
	ImGui::SameLine();
	ImGui::BeginGroup();
	ImGui::Text("%s", m_BasicMaterial->getFileName().c_str());
	if (ImGui::Button(ICON_ROOTEX_EXTERNAL_LINK "##Particles Material"))
	{
		EventManager::GetSingleton()->call("OpenModel", "EditorOpenFile", m_BasicMaterial->getFileName());
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Particles Material"))
	{
		igfd::ImGuiFileDialog::Instance()->OpenModal("Particles Material", "Choose Material", ".rmat", "game/assets/materials/");
	}
	ImGui::SameLine();
	if (igfd::ImGuiFileDialog::Instance()->FileDialog("Particles Material"))
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk)
		{
			String filePathName = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetFilePathName()).generic_string();
			setMaterial(std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial(filePathName)));
		}
		igfd::ImGuiFileDialog::Instance()->CloseDialog("Particles Material");
	}
	ImGui::SameLine();
	if (ImGui::Button((ICON_ROOTEX_REFRESH "##Particles Material")))
	{
		setMaterial(std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetDefaultMaterial()));
	}
	ImGui::EndGroup();

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
	
	ImGui::Text("Particle");
	ImGui::DragFloat3("Velocity", &m_ParticleTemplate.velocity.x);
	ImGui::DragFloat("Velocity Variation", &m_ParticleTemplate.velocityVariation);
	ImGui::DragFloat("Angular Velocity Variation", &m_ParticleTemplate.angularVelocityVariation);
	ImGui::ColorEdit4("Color Begin", &m_ParticleTemplate.colorBegin.x);
	ImGui::ColorEdit4("Color End", &m_ParticleTemplate.colorEnd.x);
	ImGui::DragFloat("Size Begin", &m_ParticleTemplate.sizeBegin, 0.01f);
	ImGui::DragFloat("Size End", &m_ParticleTemplate.sizeEnd, 0.01f);
	ImGui::DragFloat("Size Variation", &m_ParticleTemplate.sizeVariation, 0.01f);
	ImGui::DragFloat("Lifetime", &m_ParticleTemplate.lifeTime, 0.01f);
}
#endif // ROOTEX_EDITOR
