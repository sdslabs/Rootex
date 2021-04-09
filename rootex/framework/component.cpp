#include "component.h"

#include "entity.h"
#include "ecs_factory.h"

Component::Component(Entity& owner)
    : m_Owner(&owner)
{
}

bool Component::resolveDependencies()
{
	for (auto& dependency : m_Dependencies)
	{
		Component* dependedOnComponent = m_Owner->getComponentFromID(dependency->getID());
		dependency->setComponent(dependedOnComponent);

		if (!dependency->isValid())
		{
			WARN(m_Owner->getFullName() + " needs a " + ECSFactory::GetComponentNameByID(dependency->getID()) + " to use a " + getName());
			return false;
		}
	}
	return true;
}

bool Component::setup()
{
	return resolveDependencies() && setupData();
}

JSON::json Component::getJSON() const
{
	return JSON::json::object();
}

void Component::draw()
{
	ImGui::Text("Component data not available");
}
