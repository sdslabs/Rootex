#include "trigger_system.h"

#include "components/physics/trigger_component.h"
#include "ecs_factory.h"

TriggerSystem::TriggerSystem()
    : System("TriggerSystem", UpdateOrder::PostUpdate, true)
{
}

TriggerSystem* TriggerSystem::GetSingleton()
{
	static TriggerSystem singleton;
	return &singleton;
}

void TriggerSystem::update(float deltaMilliseconds)
{
	for (auto& trigger : ECSFactory::GetAllTriggerComponent())
	{
		if (!(trigger.getOwner().getScene()->getIsScenePaused() && m_IsSystemPaused))
		{
			trigger.openRegister();
			{
				btGhostObject* triggerGhost = trigger.getGhostObject();
				for (int i = 0; i < triggerGhost->getNumOverlappingObjects(); i++)
				{
					if (trigger.canNotifyEntry())
					{
						trigger.notifyEntry();
					}
					CollisionComponent* entrantComponent = (CollisionComponent*)triggerGhost->getOverlappingObject(i)->getUserPointer();
					trigger.registerEntry(entrantComponent->getOwner().getID());
				}

				int exitCount = trigger.findExitCount();
				for (int i = 0; i < exitCount; i++)
				{
					if (trigger.canNotifyExit())
					{
						trigger.notifyExit();
					}
				}
			}
			trigger.closeRegister();

			trigger.updateTransform();
		}
	}
}
