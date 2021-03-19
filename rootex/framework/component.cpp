#include "component.h"

#include "entity.h"

Component::Component()
    : m_Owner(nullptr)
{
}

Component::~Component()
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
			WARN("Could not resolve dependency of " + getName() + " on entity " + m_Owner->getFullName());
			return false;
		}
	}
	return true;
}

bool Component::setup()
{
	return resolveDependencies() && setupData();
}

bool Component::setupData()
{
	return true;
}

bool Component::setupEntities()
{
	return true;
}

void Component::onRemove()
{
}

Entity* Component::getOwner() const
{
	return m_Owner;
}

JSON::json Component::getJSON() const
{
	return {};
}

void Component::draw()
{
	ImGui::Text("Component data not available");
}
