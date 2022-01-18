#include "player_system.h"

#include "components/game/player_controller.h"

PlayerSystem::PlayerSystem()
    : System("PlayerSystem", UpdateOrder::PostUpdate, true)
{
}

PlayerSystem* PlayerSystem::GetSingleton()
{
	static PlayerSystem singleton;
	return &singleton;
}

bool PlayerSystem::initialize(const JSON::json& systemData)
{
	return false;
}

void PlayerSystem::setConfig(const SceneSettings& sceneSettings)
{
}

void PlayerSystem::begin()
{
}

void PlayerSystem::update(float deltaMilliseconds)
{
	if (!(m_IsSystemPaused))
	{
		for (auto& p : ECSFactory::GetAllPlayerController())
		{
			p.update(deltaMilliseconds);
		}
	}
	else
	{
		for (auto& p : ECSFactory::GetAllPlayerController())
		{
			if (!(p.getOwner().getScene()->getIsScenePaused()))
			{
				p.update(deltaMilliseconds);
			}
		}
	}
}

void PlayerSystem::end()
{
}
