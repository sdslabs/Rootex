#include "input_system.h"

#include "core/input/input_manager.h"

InputSystem::InputSystem()
    : System("InputSystem", UpdateOrder::Input, true)
{
	BIND_EVENT_MEMBER_FUNCTION("WindowResized", InputSystem::windowResized);
}

Variant InputSystem::windowResized(const Event* event)
{
	const Vector2& newSize = Extract(Vector2, event->getData());
	InputManager::GetSingleton()->setDisplaySize(newSize);
	return true;
}

InputSystem* InputSystem::GetSingleton()
{
	static InputSystem singleton;
	return &singleton;
}

bool InputSystem::initialize(const JSON::json& systemData)
{
	InputManager::GetSingleton()->initialize(systemData["width"], systemData["height"]);
	InputManager::GetSingleton()->loadSchemes(systemData["inputSchemes"]);
	InputManager::GetSingleton()->setScheme(systemData["startScheme"]);
	return true;
}

void InputSystem::update(float deltaMilliseconds)
{
	InputManager::GetSingleton()->update();
}
