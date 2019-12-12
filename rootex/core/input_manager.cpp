#include "input_manager.h"

//TODO is key pressed
//TODO capturing the left out key_down calls
// Where to have the switch case for mouse clicks ??
// SERIOUS SAARE FUNCTIONS BANA DIYE HAI LEKIN SWITCH CASE KAHA AAYEGA

//switch(msg.message)
//{
//	case WM_LBUTTONDBLCLK : //trigger double left click event
//								break;
//	case WM_LBUTTONDOWN		:	mouseButtonChange(1,true);
//								break;
//	case WM_LBUTTONUP		:	mouseButtonChange(1,false);
//								break;
//	case WM_MBUTTONDBLCLK	:	//trigger double middle click event
//								break;
//	case WM_MBUTTONDOWN		:	mouseButtonChange(3,true);
//								break;
//	case WM_MBUTTONUP		:	mouseButtonChange(3,false);
//								break;
//	case WM_RBUTTONDBLCLK	:	trigger double right click event
//								break;
//	case WM_RBUTTONDOWN		:	mouseButtonChange(2,true);
//								break;
//	case WM_RBUTTONUP		:	mouseButtonChange(2,false);
//								break;
//	case WM_MOUSEMOVE		: mouseMove(lparam);
//								break;
//}

InputManager* InputManager::getInputManager()
{
	if (inputManager == NULL)
	{
		inputManager = new InputManager();
	}
	return (inputManager);
}

InputManager::keyPressed(uint32_t Wparam)
{
	if (m_IsEnabled == false)
		return;

	if (m_KeysToKeybindings.find(Wparam) == m_KeysToKeybindings.end())
	{
		return;
	}
	else
	{
		KeybindingList keybindings = m_KeysToKeybindings[Wparam];
		for (auto keybinding = keybindings.begin(); keybinding != keybindings.end(); ++keybinding)
		{
			if (m_KeybindingToEventType.find(keybinding) == m_KeybindingToEventType.end())
			{
				return;
			}
			else
			{
				EventList events = m_KeybindingToEventType[keybinding];
				for (auto event = events.begin(); event != events.end(); ++event)
				{
					if (event.second)
						;
					//make a call
					else
						;
					//make a deffered call
				}
			}
		}
	}
}

//TODO
bool InputManager::isKeyPressed(const uint32_t key)
{
	//still not clear what to use getkeystate or getasynckeystate ??
	return false;
}

void InputManager::addKeyBindingToKey(const uint32_t key, const std::string& keyBinding)
{
	if (m_KeysToKeybindings.find(key) != m_KeysToKeybindings.end())
	{
		if (std::find(m_KeysToKeybindings[key].begin(), m_KeysToKeybindings[key].end(), keyBinding) == m_KeysToKeybindings[key].end())
			m_KeysToKeybindings[key].push_back(keyBinding);
	}
	else
	{
		KeybindingList keyBindings.push_back(keyBinding);
		m_KeysToKeybindings.insert({ key, keyBindings });
	}
}

void InputManager::addEventToKeyBinding(const std::string& keyBinding, const uint32_t Event, bool priority = false)
{
	if (m_KeybindingToEventType.find(keyBinding) != m_KeybindingToEventType.end())
	{
		if (std::find(m_KeybindingToEventType[keyBinding].begin(), m_KeybindingToEventType[keyBinding].end(), std::make_pair(Event,priority) == m_KeybindingToEventType[keyBinding].end())
			m_KeybindingToEventType[keyBinding].push_back(std::make_pair(Event,priority));
	}
	else
	{
		EventList events.push_back(std::make_pair(Event, priority));
		m_KeybindingToEventType.insert({ keyBinding, events });
	}
}

void InputManager::mouseMove(uint32_t lparam)
{
	xPos = (float)GET_X_LPARAM(lParam);
	yPos = (float)GET_Y_LPARAM(lParam);
}

void InputManager::mouseButtonChange(int button, bool state)
{
	m_MouseButtonMap[button] = state;
	if (state)
	{
		//trigger on press event of that button
	}
	else
	{
		//trigger on release event of that button
	}
}

void InputManager::addMouseButton(int button)
{
	m_MouseButtonMap[button]=false;
}

void InputManager::addEventToMouseButton(const int button, const uint32_t Event, bool state)
{

		if (m_MouseEventMap[button].first == Event &&  m_MouseEventMap[button].second == state)
		m_MouseEventMap[button]=std::make_pair(Event,state);
	
}



std::pair<float, float> InputManager::getCursorPosition()
{
	return std::make_pair(xpos, ypos);
}

bool InputManager::isMouseButtonPressed(const EMouseButtons& buttonName)
{
	if (m_IsEnabled == true)
	{
		return MouseButtonMap[(int)buttonName];
	}

	return false;
}
