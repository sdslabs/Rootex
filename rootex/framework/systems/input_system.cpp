#include "input_system.h"

#include "core/input/input_manager.h"

InputSystem::InputSystem()
    : System("InputSystem", UpdateOrder::Input, true)
{
	BIND_EVENT_MEMBER_FUNCTION("WindowResized", InputSystem::windowResized);
}

Variant InputSystem::windowResized(const Event* event)
{
	const Vector2& newSize = Extract<Vector2>(event->getData());
	InputManager::GetSingleton()->setDisplaySize(newSize);
	return true;
}

InputSystem* InputSystem::GetSingleton()
{
	static InputSystem singleton;
	return &singleton;
}

void InputSystem::loadSchemes(const HashMap<String, InputScheme>& schemes)
{
	InputManager::GetSingleton()->setSchemes(schemes);
}

void InputSystem::setScheme(const String& scheme)
{
	if (!m_SchemeLock)
	{
		InputManager::GetSingleton()->setScheme(scheme);
	}
}

void InputSystem::setSchemeLock(bool enabled)
{
	m_SchemeLock = enabled;
}

bool InputSystem::initialize(const JSON::json& systemData)
{
	InputManager::GetSingleton()->initialize(systemData["width"], systemData["height"]);
	return true;
}

void InputSystem::setConfig(const SceneSettings& sceneSettings)
{
	loadSchemes(sceneSettings.inputSchemes);
	setScheme(sceneSettings.startScheme);
}

void InputSystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	InputManager::GetSingleton()->update();
}
