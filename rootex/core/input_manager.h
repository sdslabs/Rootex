#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

class InputManager
{
public:
	static InputManager* getInputManager();
	keyPressed(uint32_t Wparam);
	bool isKeyPressed(const uint32_t key);
	void addKeyBindingToKey(const uint32_t key, const std::string& keyBinding);
	void addEventToKeyBinding(const std::string& keyBinding, const uint32_t Event, bool priority);
	void mouseMove(uint32_t lparam);
	void mouseButtonChange(int button, bool state);
	void addMouseButton(int button);
	bool isMouseButtonPressed(const EMouseButtons& buttonName);
	void enable() { m_IsEnabled = true; }
	void disable() { m_IsEnabled = false; }

private:
	static InputManager* inputManager;
	InputManager()
	{
		m_IsEnabled = true;
		xpos = 0;
		ypos = 0;
	}
	~InputManager() = default;
	bool m_IsEnabled;
	typedef std::vector<std::string> KeybindingList;
	typedef std::vector<std::pair<uint32_t, bool>> EventList; //the boolean indicates whether to place call or deffered call
	std::unordered_map<uint32_t, KeybindingList> m_KeysToKeybindings;
	std::unordered_map<std::string, EventList> m_KeybindingToEventType; 
	static std::unordered_map<int, bool> m_MouseButtonMap;
	static std::unordered_map<int, std::pair<uint32_t, bool>> m_MouseEventMap;
	float xpos;
	float ypos;
};
InputManager* InputManager::inputManager = NULL;

enum class EMouseButtons
{
	__MOUSE_BUTTON_1 = 0,
	__MOUSE_BUTTON_2 = 1,
	__MOUSE_BUTTON_3 = 2,
	__MOUSE_BUTTON_LEFT = __MOUSE_BUTTON_1,
	__MOUSE_BUTTON_RIGHT = __MOUSE_BUTTON_2,
	__MOUSE_BUTTON_MIDDLE = __MOUSE_BUTTON_3
};

