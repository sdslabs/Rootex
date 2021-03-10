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
	j["rotationVariation"] = p.rotationVariation;
	j["angularVelocityVariation"] = p.angularVelocityVariation;
	j["sizeBegin"] = p.sizeBegin;
	j["sizeEnd"] = p.sizeEnd;
	j["sizeVariation"] = p.sizeVariation;
	j["lifeTime"] = p.lifeTime;
}

void from_json(const JSON::json& j, ParticleTemplate& p)
{
	p.velocity = j.at("velocity");
	p.colorBegin = j.at("colorBegin");
	p.colorEnd = j.at("colorEnd");
	p.velocityVariation = j.at("velocityVariation");
	p.rotationVariation = j.value("rotationVariation", DirectX::XM_PI);
	p.angularVelocityVariation = j.at("angularVelocityVariation");
	p.sizeBegin = j.at("sizeBegin");
	p.sizeEnd = j.at("sizeEnd");
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
	    (EmitMode)componentData.value("emitMode", (int)EmitMode::Point),
	    componentData.value("emitRate", 1.0f),
	    componentData.value("emitterDimensions", Vector3 { 1.0f, 1.0f, 1.0f }),
	    componentData.value("isVisible", true),
	    componentData.value("lodEnable", true),
	    componentData.value("lodBias", 1.0f),
	    componentData.value("lodDistance", 10.0f),
	    componentData.value("renderPass", (unsigned int)RenderPass::Basic));
	return particles;
}

CPUParticlesComponent::CPUParticlesComponent(
	size_t poolSize,
	const String& particleModelPath,
	const String& materialPath,
	const ParticleTemplate& particleTemplate,
	EmitMode emitMode,
	int emitRate,
	const Vector3& emitterDimensions,
	bool visibility,
    bool lodEnable,
    float lodBias,
    float lodDistance,
	unsigned int renderPass)
    : ModelComponent(
		renderPass,
		ResourceLoader::CreateModelResourceFile(particleModelPath),
		{},
		visibility,
		lodEnable,
		lodBias,
		lodDistance,
        {})
    , m_ParticlesMaterial(std::dynamic_pointer_cast<ParticlesMaterial>(MaterialLibrary::GetMaterial(materialPath)))
    , m_ParticleTemplate(particleTemplate)
    , m_CurrentEmitMode(emitMode)
    , m_EmitterDimensions(emitterDimensions)
    , m_EmitRate(emitRate)
{
	m_InstanceBufferData.resize(MAX_PARTICLES);
	m_InstanceBuffer.reset(new VertexBuffer(m_InstanceBufferData));
	expandPool(poolSize);
}

bool CPUParticlesComponent::setupData()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>();
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

	const float delta = deltaMilliseconds * MS_TO_S;
	m_EmitCount += m_EmitRate;
	while (m_EmitCount > 0.0f)
	{
		emit(m_ParticleTemplate);
		m_EmitCount--;
	}
	{
		ZoneNamedN(particleInterpolation, "Particle Interpolation", true);

		// Particle animation
		for (int i = 0; i < m_ParticlePool.size(); i++)
		{
			Particle& particle = m_ParticlePool[i];

			particle.lifeRemaining -= delta;

			if (particle.lifeRemaining < 0.0f)
			{
				continue;
			}

			float life = particle.lifeRemaining / particle.lifeTime;
			float size = particle.sizeBegin * (life) + particle.sizeEnd * (1.0f - life);

			particle.position = particle.position + particle.velocity * delta;
			particle.rotation = Quaternion::Concatenate(
			    particle.rotation,
			    Quaternion::CreateFromYawPitchRoll(
			        particle.angularVelocity.x * delta,
			        particle.angularVelocity.y * delta,
			        particle.angularVelocity.z * delta));

			InstanceData& instance = m_InstanceBufferData[i];
			instance.transform = Matrix::CreateScale(size)
			    * Matrix::CreateFromQuaternion(particle.rotation)
			    * Matrix::CreateTranslation(particle.position);
			instance.inverseTransposeTransform = instance.transform.Invert().Transpose();
			instance.color = Color::Lerp(particle.colorEnd, particle.colorBegin, life);
		}

		// Count the size of the final live buffer
		m_LiveParticlesCount = 0;
		for (auto& particle : m_ParticlePool)
		{
			if (particle.lifeRemaining > 0.0f)
			{
				m_LiveParticlesCount++;
			}
		}

		// Copy live particles to the buffer which will be sent to the GPU
		if (m_InstanceBufferLiveData.size() < m_LiveParticlesCount)
		{
			m_InstanceBufferLiveData.resize(m_LiveParticlesCount);
		}
		int liveCount = 0;
		for (int i = 0; i < m_ParticlePool.size(); i++)
		{
			if (m_ParticlePool[i].lifeRemaining > 0.0f)
			{
				m_InstanceBufferLiveData[liveCount] = m_InstanceBufferData[i];
				liveCount++;
			}
		}
	}
	return true;
}

void CPUParticlesComponent::render(float viewDistance)
{
	ZoneScoped;

	RenderSystem::GetSingleton()->getRenderer()->bind(m_ParticlesMaterial.get());
	m_InstanceBuffer->setData(m_InstanceBufferLiveData);
	for (auto& [material, meshes] : m_ModelResourceFile->getMeshes())
	{
		for (auto& mesh : meshes)
		{
			RenderSystem::GetSingleton()->getRenderer()->drawInstanced(mesh.m_VertexBuffer.get(), mesh.getLOD(getLODFactor(viewDistance)).get(), m_InstanceBuffer.get(), m_LiveParticlesCount);
		}
	}
}

void CPUParticlesComponent::setMaterial(Ref<ParticlesMaterial> particlesMaterial)
{
	if (!particlesMaterial)
	{
		WARN("Particles was tried to set to nullptr. Reverted to previously set material");
		return;
	}

	m_ParticlesMaterial = particlesMaterial;
}

void CPUParticlesComponent::emit(const ParticleTemplate& particleTemplate)
{
	ZoneScoped;

	Particle& particle = m_ParticlePool[m_PoolIndex];

	particle.velocity = particleTemplate.velocity + particleTemplate.velocityVariation * 2.0f * Vector3 { Random::Float() - 0.5f, Random::Float() - 0.5f, Random::Float() - 0.5f };
	particle.angularVelocity = 2.0f * Vector3(Random::Float() - 0.5f, Random::Float() - 0.5f, Random::Float() - 0.5f) * particleTemplate.angularVelocityVariation;
	particle.angularVelocity.Normalize();

	particle.colorBegin = particleTemplate.colorBegin;
	particle.colorEnd = particleTemplate.colorEnd;

	particle.lifeTime = particleTemplate.lifeTime;
	particle.lifeRemaining = particleTemplate.lifeTime;
	particle.sizeBegin = particleTemplate.sizeBegin * (1.0f + particleTemplate.sizeVariation * 2.0f * (Random::Float() - 0.5f));
	particle.sizeEnd = particleTemplate.sizeEnd;

	Matrix initialTransform = Matrix::Identity;
	Vector3 position;
	switch (m_CurrentEmitMode)
	{
	case EmitMode::Point:
		break;
	case EmitMode::Square:
		position = {
			2.0f * (Random::Float() - 0.5f) * m_EmitterDimensions.x,
			2.0f * (Random::Float() - 0.5f) * m_EmitterDimensions.y,
			0.0f
		};
		break;
	case EmitMode::Cube:
		position = {
			2.0f * (Random::Float() - 0.5f) * m_EmitterDimensions.x,
			2.0f * (Random::Float() - 0.5f) * m_EmitterDimensions.y,
			2.0f * (Random::Float() - 0.5f) * m_EmitterDimensions.z
		};
		break;
	case EmitMode::Sphere:
		position = Vector3::Transform(
		    Vector3 { m_EmitterDimensions.x, 0.0f, 0.0f },
		    Quaternion::CreateFromYawPitchRoll(
		        2.0f * (Random::Float() - 0.5f) * DirectX::XM_PI,
		        2.0f * (Random::Float() - 0.5f) * DirectX::XM_PI,
		        2.0f * (Random::Float() - 0.5f) * DirectX::XM_PI));
		break;
	default:
		break;
	}
	initialTransform = Matrix::CreateTranslation(position);

	Quaternion rotation = Quaternion::CreateFromYawPitchRoll(
	    particleTemplate.rotationVariation * Random::Float(),
	    particleTemplate.rotationVariation * Random::Float(),
	    particleTemplate.rotationVariation * Random::Float());
	initialTransform = Matrix::CreateFromQuaternion(rotation) * initialTransform;
	m_InstanceBufferData[m_PoolIndex].transform = initialTransform * m_TransformComponent->getAbsoluteTransform();

	particle.position = position + m_TransformComponent->getAbsolutePosition();
	particle.rotation = Quaternion::Concatenate(rotation, m_TransformComponent->getAbsoluteRotation());
	particle.scale = m_TransformComponent->getAbsoluteScale();

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}

void CPUParticlesComponent::expandPool(const size_t& poolSize)
{
	if (poolSize < 1)
	{
		WARN("Cancelled attempt to add too little particles in: " + m_Owner->getFullName());
		return;
	}
	if (poolSize > MAX_PARTICLES)
	{
		WARN("Cancelled attempt to add too many particles in: " + m_Owner->getFullName());
		return;
	}

	m_ParticlePool.resize(poolSize);
	m_InstanceBufferData.resize(poolSize);
	m_PoolIndex = poolSize - 1;
}

JSON::json CPUParticlesComponent::getJSON() const
{
	JSON::json& j = ModelComponent::getJSON();

	j["materialPath"] = m_ParticlesMaterial->getFileName();
	j["poolSize"] = m_ParticlePool.size();
	j["particleTemplate"] = m_ParticleTemplate;
	j["emitMode"] = (int)m_CurrentEmitMode;
	j["emitterDimensions"]["x"] = m_EmitterDimensions.x;
	j["emitterDimensions"]["y"] = m_EmitterDimensions.y;
	j["emitterDimensions"]["z"] = m_EmitterDimensions.z;

	return j;
}

void CPUParticlesComponent::draw()
{
	ImGui::Text("Model");
	ModelComponent::draw();

	ImGui::Separator();

	ImGui::Text("%s", "Particles Material");
	ImGui::Image(m_ParticlesMaterial->getPreview(), { 50, 50 });
	ImGui::SameLine();
	ImGui::BeginGroup();
	ImGui::Text("%s", m_ParticlesMaterial->getFileName().c_str());
	if (ImGui::Button(ICON_ROOTEX_SEARCH "##Particles Material"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, m_ParticlesMaterial->getFileName());
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Particles Material"))
	{
		if (Optional<String> result = OS::SelectFile("Material(*.rmat)\0*.rmat\0", "game/assets/materials/"))
		{
			setMaterial(std::dynamic_pointer_cast<ParticlesMaterial>(MaterialLibrary::GetMaterial(*result)));
		}
	}
	ImGui::SameLine();
	if (ImGui::Button((ICON_ROOTEX_REFRESH "##Particles Material")))
	{
		setMaterial(std::dynamic_pointer_cast<ParticlesMaterial>(MaterialLibrary::GetDefaultParticlesMaterial()));
	}
	ImGui::EndGroup();

	ImGui::Separator();

	ImGui::Text("Emitter");
	ImGui::Combo("Emit Mode", (int*)&m_CurrentEmitMode, "Point\0Square\0Cube\0Sphere");
	switch (m_CurrentEmitMode)
	{
	case CPUParticlesComponent::EmitMode::Point:
		break;
	case CPUParticlesComponent::EmitMode::Square:
		ImGui::DragFloat2("Dimensions", &m_EmitterDimensions.x, 0.01f, 0.0f);
		break;
	case CPUParticlesComponent::EmitMode::Cube:
		ImGui::DragFloat3("Dimensions", &m_EmitterDimensions.x, 0.01f, 0.0f);
		break;
	case CPUParticlesComponent::EmitMode::Sphere:
		ImGui::DragFloat("Dimensions", &m_EmitterDimensions.x, 0.01f, 0.0f);
		break;
	}
	int poolSize = m_ParticlePool.size();
	if (ImGui::DragInt("Pool Size", &poolSize, 1.0f, 1, MAX_PARTICLES))
	{
		expandPool(poolSize);
	}
	ImGui::DragFloat("Emit Rate", &m_EmitRate, 0.1f, 0.0f, FLT_MAX);

	ImGui::Separator();

	ImGui::Text("Particle");
	ImGui::DragFloat3("Velocity", &m_ParticleTemplate.velocity.x, 0.01f);
	ImGui::DragFloat("Velocity Variation", &m_ParticleTemplate.velocityVariation, 0.01f, 0.0f);
	ImGui::SliderAngle("Rotation Variation", &m_ParticleTemplate.rotationVariation, 0.0f, 180.0f);
	ImGui::DragFloat("Angular Velocity Variation", &m_ParticleTemplate.angularVelocityVariation, 0.01f, 0.0f);
	ImGui::ColorEdit4("Color Begin", &m_ParticleTemplate.colorBegin.x);
	ImGui::ColorEdit4("Color End", &m_ParticleTemplate.colorEnd.x);
	ImGui::DragFloat("Size Begin", &m_ParticleTemplate.sizeBegin, 0.01f);
	ImGui::DragFloat("Size End", &m_ParticleTemplate.sizeEnd, 0.01f);
	ImGui::DragFloat("Size Variation", &m_ParticleTemplate.sizeVariation, 0.01f);
	ImGui::DragFloat("Lifetime", &m_ParticleTemplate.lifeTime, 0.01f);
}
