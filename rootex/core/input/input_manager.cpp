#include "input_manager.h"
#include "event_manager.h"

#include <functional>

void InputManager::initialize(unsigned int width, unsigned int height)
{
	m_GainputManager.SetDisplaySize(width, height);
	m_Width = width;
	m_Height = height;

	DeviceIDs[Device::Mouse] = m_GainputManager.CreateDevice<gainput::InputDeviceMouse>();
	DeviceIDs[Device::Keyboard] = m_GainputManager.CreateDevice<gainput::InputDeviceKeyboard>();
	DeviceIDs[Device::Pad1] = m_GainputManager.CreateDevice<gainput::InputDevicePad>();
	DeviceIDs[Device::Pad2] = m_GainputManager.CreateDevice<gainput::InputDevicePad>();

	m_Listener.setID(1);
	setEnabled(true);
}

void InputManager::setEnabled(bool enabled)
{
	if (enabled)
	{
		PRINT("Input enabled");
		enableScheme(m_CurrentInputScheme, enabled);
		m_GainputMap.AddListener(&m_Listener);
	}
	else
	{
		PRINT("Input disabled");
		m_GainputMap.Clear();
		m_GainputManager.RemoveListener(m_Listener.getID());
	}
	m_IsEnabled = enabled;
}

void InputManager::addScheme(const String& name, const InputScheme& inputScheme)
{
	if (m_InputSchemes.find(name) != m_InputSchemes.end())
	{
		WARN("Overriding an already existing input scheme: " + name);
	}

	m_InputSchemes[name] = inputScheme;
}

void InputManager::enableScheme(const String& schemeName, bool enabled)
{
	if (schemeName.empty())
	{
		return;
	}

	m_CurrentInputScheme = schemeName;

	const InputScheme& scheme = m_InputSchemes.at(schemeName);
	if (enabled)
	{
		for (auto& boolBinding : scheme.bools)
		{
			mapBool(boolBinding.inputEvent, boolBinding.device, boolBinding.button);
		}
		for (auto& floatBinding : scheme.floats)
		{
			mapFloat(floatBinding.inputEvent, floatBinding.device, floatBinding.button);
		}
	}
	else
	{
		for (auto& boolBinding : scheme.bools)
		{
			unmap(boolBinding.inputEvent);
		}
		for (auto& floatBinding : scheme.floats)
		{
			unmap(floatBinding.inputEvent);
		}
	}
}

void InputManager::clearSchemes()
{
	m_InputEventIDNames.clear();
	m_InputEventNameIDs.clear();
	m_InputSchemes.clear();
	m_GainputMap.Clear();
}

void InputManager::mapBool(const Event::Type& action, Device device, DeviceButtonID button)
{
	m_InputEventNameIDs[action] = getNextID();
	m_InputEventIDNames[m_InputEventNameIDs[action]] = action;
	if (!m_GainputMap.MapBool((gainput::UserButtonId)m_InputEventNameIDs[action], DeviceIDs[device], button))
	{
		WARN("Bool mapping could not done: " + action);
	}
}

void InputManager::mapFloat(const Event::Type& action, Device device, DeviceButtonID button)
{
	m_InputEventNameIDs[action] = getNextID();
	m_InputEventIDNames[m_InputEventNameIDs[action]] = action;
	if (!m_GainputMap.MapFloat((gainput::UserButtonId)m_InputEventNameIDs[action], DeviceIDs[device], button))
	{
		WARN("Float mapping could not done: " + action);
	}
}

void InputManager::unmap(const Event::Type& action)
{
	m_GainputMap.Unmap(m_InputEventNameIDs[action]);
}

bool InputManager::isPressed(const Event::Type& action)
{
	return m_IsEnabled && m_GainputMap.GetBool((gainput::UserButtonId)m_InputEventNameIDs[action]);
}

bool InputManager::wasPressed(const Event::Type& action)
{
	return m_IsEnabled && m_GainputMap.GetBoolWasDown((gainput::UserButtonId)m_InputEventNameIDs[action]);
}

bool InputManager::hasPressed(const Event::Type& action)
{
	return m_IsEnabled && m_GainputMap.GetBoolIsNew((gainput::UserButtonId)m_InputEventNameIDs[action]);
}

float InputManager::getFloat(const Event::Type& action)
{
	if (m_IsEnabled)
	{
		return m_GainputMap.GetFloat((gainput::UserButtonId)m_InputEventNameIDs[action]);
	}
	return 0;
}

float InputManager::getFloatDelta(const Event::Type& action)
{
	if (m_IsEnabled)
	{
		return m_GainputMap.GetFloatDelta((gainput::UserButtonId)m_InputEventNameIDs[action]);
	}
	return 0;
}

void InputManager::update()
{
	m_GainputManager.Update();
}

void InputManager::setDisplaySize(const Vector2& newSize)
{
	m_GainputManager.SetDisplaySize(newSize.x, newSize.y);
}

unsigned int InputManager::getNextID()
{
	static unsigned int count = 0;
	return count++;
}

InputManager* InputManager::GetSingleton()
{
	static InputManager singleton;
	return &singleton;
}

bool InputManager::BoolListen(int userButton, bool oldValue, bool newValue)
{
	EventManager::GetSingleton()->call(GetSingleton()->m_InputEventIDNames[userButton], Vector2(oldValue, newValue));
	return true;
}

bool InputManager::FloatListen(int userButton, float oldValue, float newValue)
{
	EventManager::GetSingleton()->call(GetSingleton()->m_InputEventIDNames[userButton], Vector2(oldValue, newValue));
	return true;
}

InputManager::InputManager()
    : m_GainputMap(m_GainputManager)
    , m_Listener(BoolListen, FloatListen)
    , m_Width(0)
    , m_Height(0)
{
}

void InputManager::forwardMessage(const MSG& msg)
{
	m_GainputManager.HandleMessage(msg);
}

void to_json(JSON::json& j, const InputDescription& s)
{
	j["device"] = (int)s.device;
	j["button"] = (int)s.button;
	j["inputEvent"] = s.inputEvent;
}

void from_json(const JSON::json& j, InputDescription& s)
{
	s.device = (Device)(int)j["device"];
	s.button = (int)j["button"];
	s.inputEvent = j["inputEvent"];
}

void to_json(JSON::json& j, const InputScheme& s)
{
	j["bools"] = s.bools;
	j["floats"] = s.floats;
}

void from_json(const JSON::json& j, InputScheme& s)
{
	for (auto& boolInput : j["bools"])
	{
		InputDescription id = boolInput;
		s.bools.push_back(id);
	}
	for (auto& floatInput : j["floats"])
	{
		InputDescription id = floatInput;
		s.floats.push_back(id);
	}
}
