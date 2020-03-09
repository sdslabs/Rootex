#include "component.h"

Component::Component()
    : m_Owner(nullptr)
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
