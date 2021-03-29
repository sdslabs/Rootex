#include "player_controller.h"

Ptr<Component> PlayerController::Create(const JSON::json& componentData)
{
	return std::make_unique<PlayerController>();
}

PlayerController::PlayerController()
{
}

bool PlayerController::setupData()
{
	if (!m_Owner->addComponent("TransformComponent", {})
	    && !m_Owner->addComponent("AnimatedModelComponent", {})
	    && !m_Owner->addComponent("CapsuleColliderComponent", {}))
	{
		WARN("Could not add sub-components to entity: " + m_Owner->getName());
		return false;
	}

	return true;
}

JSON::json PlayerController::getJSON() const
{
	return JSON::json();
}

void PlayerController::draw()
{
}
