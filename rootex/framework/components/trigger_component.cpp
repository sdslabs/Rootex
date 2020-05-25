#include "trigger_component.h"

#include "entity_factory.h"

Component* TriggerComponent::Create(const JSON::json& componentData)
{
	return new TriggerComponent(componentData["targetEntityID"]);
}

Component* TriggerComponent::CreateDefault()
{
	return new TriggerComponent(INVALID_ID);
}

TriggerComponent::TriggerComponent(EntityID targetEntity)
    : m_TargetEntityID(targetEntity)
{
}

bool TriggerComponent::setupEntities()
{
	if (m_TargetEntityID != INVALID_ID)
	{
		Ref<Entity> targetEntity = EntityFactory::GetSingleton()->findEntity(m_TargetEntityID);

		if (!targetEntity)
		{
			ERR("Target entity not found: " + std::to_string(m_TargetEntityID));
			return false;
		}

		m_TargetEntityTrigger = targetEntity->getComponent<TriggerComponent>();
		if (!m_TargetEntityTrigger)
		{
			ERR("TriggerComponent target " + m_Owner->getFullName() + " does not have a TriggerComponent");
			return false;
		}
	}
	return true;
}

void TriggerComponent::setTarget(Ref<Entity> entity)
{
	m_TargetEntityTrigger = entity->getComponent<TriggerComponent>();

	if (m_TargetEntityTrigger)
	{
		m_TargetEntityID = entity->getID();
	}
	else
	{
		WARN("TriggerComponent not found on new target for entity " + m_Owner->getFullName());
		m_TargetEntityID = INVALID_ID;
	}
}

void TriggerComponent::trigger()
{
	for (auto& component : m_Owner->getAllComponents())
	{
		component.second->onTrigger();
	}
	
	if (m_TargetEntityTrigger)
	{
		m_TargetEntityTrigger->trigger();
	}
}

JSON::json TriggerComponent::getJSON() const
{
	JSON::json j;

	j["targetEntityID"] = m_TargetEntityID;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "systems/render_system.h"
void TriggerComponent::draw()
{
	static bool showTarget = false;
	ImGui::Checkbox("Show Target", &showTarget);

	if (m_TargetEntityTrigger && showTarget)
	{
		TransformComponent* targetTransform = m_TargetEntityTrigger->getOwner()->getComponent<TransformComponent>().get();
		TransformComponent* triggerTransform = getOwner()->getComponent<TransformComponent>().get();

		if (targetTransform && triggerTransform)
		{
			RenderSystem::GetSingleton()->submitLine(
				triggerTransform->getAbsoluteTransform().Translation(), 
				targetTransform->getAbsoluteTransform().Translation());
		}
	}

	String preview = m_TargetEntityTrigger ? m_TargetEntityTrigger->getOwner()->getFullName(): "None";
	if (ImGui::BeginCombo("Target", preview.c_str()))
	{
		for (auto& entity : EntityFactory::GetSingleton()->getEntities())
		{
			if (ImGui::Selectable(entity.second->getFullName().c_str()))
			{
				setTarget(entity.second);
			}
		}

		ImGui::EndCombo();
	}
}
#endif // ROOTEX_EDITOR
