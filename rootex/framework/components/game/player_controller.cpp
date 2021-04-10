#include "player_controller.h"

DEFINE_COMPONENT(PlayerController);

PlayerController::PlayerController(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_DependencyOnAnimatedModelComponent(this)
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
