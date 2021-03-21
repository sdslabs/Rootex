#include "particle_effect_component.h"

#include "core/resource_loader.h"
#include "systems/particle_system.h"

Ptr<Component> ParticleEffectComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<ParticleEffectComponent>(
	    ResourceLoader::CreateParticleEffectResourceFile(componentData.value("resFile", "rootex/assets/toon.efkefc")),
	    componentData.value("playOnStart", false),
	    componentData.value("isMoving", false),
	    componentData.value("startFrame", 0),
	    componentData.value("useSpeed", false),
	    componentData.value("speed", 1.0f),
	    componentData.value("useTarget", false),
	    componentData.value("target", Vector3::Zero));
}

ParticleEffectComponent::ParticleEffectComponent(Ref<ParticleEffectResourceFile> effect, bool playOnStart, bool isMoving, int startFrame, bool useSpeed, float speed, bool useTarget, const Vector3& targetLocation)
    : m_EffectHandle(0)
    , m_EffectResource(effect)
    , m_StartFrame(startFrame)
    , m_PlayOnStart(playOnStart)
    , m_IsMoving(isMoving)
    , m_UseSpeed(useSpeed)
    , m_Speed(speed)
    , m_UseTarget(useTarget)
    , m_TargetLocation(targetLocation)
    , m_DependencyOnTransformComponent(this)
{
}

bool ParticleEffectComponent::isPaused() const
{
	return ParticleSystem::GetSingleton()->getPaused(m_EffectHandle);
}

void ParticleEffectComponent::setPlaying(bool enabled)
{
	if (enabled)
	{
		m_EffectHandle = ParticleSystem::GetSingleton()->play(m_EffectResource->getEffect(), m_TransformComponent->getAbsolutePosition(), m_StartFrame);
		ParticleSystem::GetSingleton()->setMatrix(m_EffectHandle, m_TransformComponent->getAbsoluteTransform());

		if (m_UseSpeed)
		{
			ParticleSystem::GetSingleton()->setSpeed(m_EffectHandle, m_Speed);
		}
		if (m_UseTarget)
		{
			ParticleSystem::GetSingleton()->setTargetLocation(m_EffectHandle, m_TargetLocation);
		}
	}
	else
	{
		ParticleSystem::GetSingleton()->stop(m_EffectHandle);
	}
}

void ParticleEffectComponent::play()
{
	setPlaying(true);
}

void ParticleEffectComponent::stop()
{
	setPlaying(false);
}

void ParticleEffectComponent::setEffect(Ref<ParticleEffectResourceFile> effect)
{
	m_EffectResource = effect;
}

JSON::json ParticleEffectComponent::getJSON() const
{
	JSON::json j;

	j["resFile"] = m_EffectResource->getPath().generic_string();
	j["playOnStart"] = m_PlayOnStart;
	j["isMoving"] = m_IsMoving;
	j["startFrame"] = m_StartFrame;
	j["useSpeed"] = m_UseSpeed;
	j["speed"] = m_Speed;
	j["useTarget"] = m_UseTarget;
	j["target"] = m_TargetLocation;

	return j;
}

void ParticleEffectComponent::draw()
{
	ImGui::Text("%s", m_EffectResource->getPath().generic_string().c_str());

	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::ParticleEffect), "game/assets/"))
		{
			setEffect(ResourceLoader::CreateParticleEffectResourceFile(*result));
		}
	}

	if (ImGui::Button("Play"))
	{
		play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		stop();
	}

	ImGui::DragInt("Start Frame", &m_StartFrame, 0.5f, 0, INT_MAX, "Frame %d", ImGuiSliderFlags_AlwaysClamp);
	ImGui::Checkbox("Play On Start", &m_PlayOnStart);
	ImGui::Checkbox("Moving", &m_IsMoving);

	ImGui::Checkbox("Use Speed", &m_UseSpeed);
	if (m_UseSpeed)
	{
		ImGui::DragFloat("Speed", &m_Speed, 0.01f, 0.0f, 2.0f);
		m_Speed = std::max(m_Speed, 0.01f);
	}

	ImGui::Checkbox("Use Target", &m_UseTarget);
	if (m_UseTarget)
	{
		ImGui::DragFloat3("Target", &m_TargetLocation.x, 0.01f, 0.0f, 1.0f);
	}
}
