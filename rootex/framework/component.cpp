#include "component.h"

Component::Component()
{
}

Component::~Component()
{
}

Entity* Component::getOwner() const
{
	return m_Owner.get();
}
