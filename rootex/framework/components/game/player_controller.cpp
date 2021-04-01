#include "player_controller.h"

Ptr<Component> PlayerController::Create(const JSON::json& componentData)
{
	return std::make_unique<PlayerController>();
}

PlayerController::PlayerController()
    : m_DependencyOnAnimatedModelComponent(this)
    , m_DependencyOnCapsuleColliderComponent(this)
    , m_DependencyOnTransformComponent(this)
{
}

bool PlayerController::setupData()
{
	return true;
}

JSON::json PlayerController::getJSON() const
{
	return JSON::json();
}

void PlayerController::draw()
{
}
