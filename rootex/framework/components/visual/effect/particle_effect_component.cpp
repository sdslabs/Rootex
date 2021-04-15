#include "particle_effect_component.h"

#include "core/resource_loader.h"
#include "systems/particle_system.h"

#define EFFECT_ERROR_HANDLE -1

DEFINE_COMPONENT(ParticleEffectComponent);

ParticleEffectComponent::ParticleEffectComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_EffectHandle(EFFECT_ERROR_HANDLE)
    , m_EffectResource(ResourceLoader::CreateParticleEffectResourceFile(data.value("resFile", "rootex/assets/effects/rootex.efkefc")))
    , m_StartFrame(data.value("startFrame", 0))
    , m_PlayOnStart(data.value("playOnStart", false))
    , m_IsMoving(data.value("isMoving", false))
    , m_UseSpeed(data.value("useSpeed", false))
    , m_Speed(data.value("speed", 1.0f))
    , m_UseTarget(data.value("useTarget", false))
    , m_TargetLocation(data.value("target", Vector3::Zero))
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
		if (m_EffectHandle != EFFECT_ERROR_HANDLE)
		{
			stop();
		}

		m_EffectHandle = ParticleSystem::GetSingleton()->play(m_EffectResource->getEffect(), getTransformComponent()->getAbsolutePosition(), m_StartFrame);
		ParticleSystem::GetSingleton()->setMatrix(m_EffectHandle, getTransformComponent()->getAbsoluteTransform());

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
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_REFRESH))
	{
		m_EffectResource->reimport();
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
