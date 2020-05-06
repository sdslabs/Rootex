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

bool Component::setupEntities()
{
	return true;
}

void Component::onRemove()
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
