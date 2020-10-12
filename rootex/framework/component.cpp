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
		Component* dependedOnComponent = m_Owner->getComponentFromID(dependency->getID()).get();
		dependency->setComponent(dependedOnComponent);

		if (!dependency->isValid())
		{
			ERR("Could not resolve dependency of " + getName() + " on entity " + m_Owner->getFullName());
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

void Component::onTrigger()
{
}

Ref<Entity> Component::getOwner() const
{
	return m_Owner;
}

JSON::json Component::getJSON() const
{
	return {};
}

#ifdef ROOTEX_EDITOR
void Component::draw()
{
	ImGui::Text("Component data not available");
}
#endif // ROOTEX_EDITOR
