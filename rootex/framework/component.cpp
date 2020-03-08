#include "component.h"

Component::Component()
{
}

Component::~Component()
{
}

bool Component::setup()
{
	return true;
}

Ref<Entity> Component::getOwner() const
{
	return m_Owner;
}
