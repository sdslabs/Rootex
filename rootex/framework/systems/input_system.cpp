#include "input_system.h"

#include "core/input/input_manager.h"

InputSystem::InputSystem()
    : System("InputSystem", UpdateOrder::Input, true)
{
	BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowResized, InputSystem::windowResized);
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
	for (auto& [name, scheme] : schemes)
	{
		InputManager::GetSingleton()->addScheme(name, scheme);
	}
}

void InputSystem::addScheme(const String& name, const InputScheme& inputScheme)
{
	InputManager::GetSingleton()->addScheme(name, inputScheme);
}

void InputSystem::enableScheme(const String& scheme, bool enabled)
{
	if (!m_SchemeLock)
	{
		InputManager::GetSingleton()->enableScheme(scheme, enabled);
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
	enableScheme(sceneSettings.startScheme, true);
}

void InputSystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	InputManager::GetSingleton()->update();
}
