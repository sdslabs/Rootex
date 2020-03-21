#include "component.h"

void Component::BindFunctions()
{
	luabridge::getGlobalNamespace(LuaInterpreter::GetSingleton()->getLuaState())
	    .beginNamespace("Rootex")
	    .beginClass<Component>("Component")
	    .addStaticData("ID", (unsigned int*)&s_ID, false)
	    .addFunction("getOwner", &getOwnerPointer)
	    .addFunction("getComponentID", &getComponentID)
	    .addFunction("getName", &getName)
	    .endClass()
	    .endNamespace();
}

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

void Component::onRemove()
{
}

Ref<Entity> Component::getOwner() const
{
	return m_Owner;
}

Entity* Component::getOwnerPointer() const
{
	return getOwner().get();
}

JSON::json Component::getJSON() const
{
	return {};
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void Component::draw()
{
	ImGui::Text("Component data not available");
}
#endif // ROOTEX_EDITOR
