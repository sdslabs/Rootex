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

void InputSystem::loadSchemes(const JSON::json& schemes)
{
	InputManager::GetSingleton()->loadSchemes(schemes);
}

void InputSystem::setScheme(const String& scheme)
{
	InputManager::GetSingleton()->setScheme(scheme);
}

bool InputSystem::initialize(const JSON::json& systemData)
{
	InputManager::GetSingleton()->initialize(systemData["width"], systemData["height"]);
	return true;
}

void InputSystem::setConfig(const JSON::json& configData, bool openInEditor)
{
	if (openInEditor)
	{
		return;
	}

	if (configData.find("inputSchemes") != configData.end())
	{
		loadSchemes(configData["inputSchemes"]);
		if (configData.find("startScheme") != configData.end())
		{
			setScheme(configData["startScheme"]);
		}
	}
}

void InputSystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	InputManager::GetSingleton()->update();
}
