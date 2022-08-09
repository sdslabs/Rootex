#include "input_system.h"

#include "core/input/input_manager.h"

InputSystem::InputSystem()
    : System("InputSystem", UpdateOrder::Input, true)
{
	m_Binder.bind(RootexEvents::WindowResized, this, &InputSystem::windowResized);
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
	InputManager::GetSingleton()->loadSchemes(schemes);
}

void InputSystem::addScheme(const String& name, const InputScheme& scheme)
{
	InputManager::GetSingleton()->addScheme(name, scheme);
}

void InputSystem::pushScheme(const String& name)
{
	InputManager::GetSingleton()->pushScheme(name);
}

void InputSystem::popScheme()
{
	InputManager::GetSingleton()->popScheme();
}

void InputSystem::flushSchemes()
{
	InputManager::GetSingleton()->flushSchemes();
}

bool InputSystem::initialize(const JSON::json& systemData)
{
	InputManager::GetSingleton()->initialize(systemData["width"], systemData["height"]);
	return true;
}

void InputSystem::setConfig(const SceneSettings& sceneSettings)
{
	loadSchemes(sceneSettings.inputSchemes);
	for (auto&& inputScheme : sceneSettings.inputSchemes)
	{
		if (inputScheme.second.isActive)
		{
			pushScheme(inputScheme.first);
		}
	}
}

void InputSystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	InputManager::GetSingleton()->update();
}
