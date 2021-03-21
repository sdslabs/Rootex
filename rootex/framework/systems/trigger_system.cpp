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
	for (auto& c : ECSFactory::GetComponents<TriggerComponent>())
	{
		TriggerComponent* trigger = (TriggerComponent*)c;

		trigger->openRegister();
		{
			btGhostObject* triggerGhost = trigger->getGhostObject();
			for (int i = 0; i < triggerGhost->getNumOverlappingObjects(); i++)
			{
				if (trigger->canNotifyEntry())
				{
					trigger->notifyEntry();
				}
				CollisionComponent* entrantComponent = (CollisionComponent*)triggerGhost->getOverlappingObject(i)->getUserPointer();
				trigger->registerEntry(entrantComponent->getOwner()->getScene()->getID());
			}

			int exitCount = trigger->findExitCount();
			for (int i = 0; i < exitCount; i++)
			{
				if (trigger->canNotifyExit())
				{
					trigger->notifyExit();
				}
			}
		}
		trigger->closeRegister();

		trigger->updateTransform();
	}
}
