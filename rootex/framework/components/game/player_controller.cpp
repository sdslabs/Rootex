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
	return false;
}

JSON::json PlayerController::getJSON() const
{
	return JSON::json();
}

void PlayerController::draw()
{
}
