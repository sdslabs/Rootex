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
		setScheme(m_CurrentInputScheme);
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

void InputManager::loadSchemes(const JSON::json& inputSchemes)
{
	m_InputSchemes.clear();

	for (auto& inputScheme : inputSchemes)
	{
		Vector<InputButtonBindingData> scheme;
		InputButtonBindingData button;

		for (auto& keys : inputScheme["bools"])
		{
			button.m_Type = InputButtonBindingData::Type::Bool;
			button.m_InputEvent = keys["inputEvent"];
			button.m_Device = keys["device"];
			button.m_ButtonID = keys["button"];
			scheme.emplace_back(button);
		}

		for (auto& axis : inputScheme["floats"])
		{
			button.m_Type = InputButtonBindingData::Type::Float;
			button.m_InputEvent = axis["inputEvent"];
			button.m_Device = axis["device"];
			button.m_ButtonID = axis["button"];
			scheme.emplace_back(button);
		}

		m_InputSchemes[inputScheme["name"]] = scheme;
	}
}

void InputManager::setScheme(const String& schemeName)
{
	m_GainputMap.Clear();

	const Vector<InputButtonBindingData>& scheme = m_InputSchemes[schemeName];
	for (auto& binding : scheme)
	{
		switch (binding.m_Type)
		{
		case InputButtonBindingData::Type::Bool:
			mapBool(binding.m_InputEvent, binding.m_Device, binding.m_ButtonID);
			break;
		case InputButtonBindingData::Type::Float:
			mapFloat(binding.m_InputEvent, binding.m_Device, binding.m_ButtonID);
			break;
		default:
			break;
		}
	}
	m_CurrentInputScheme = schemeName;
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
	if (m_IsEnabled)
	{
		return m_GainputMap.GetBool((gainput::UserButtonId)m_InputEventNameIDs[action]);
	}
	return false;
}

bool InputManager::wasPressed(const Event::Type& action)
{
	if (m_IsEnabled)
	{
		return m_GainputMap.GetBoolWasDown((gainput::UserButtonId)m_InputEventNameIDs[action]);
	}
	return false;
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
	EventManager::GetSingleton()->call("BoolInputEvent", GetSingleton()->m_InputEventIDNames[userButton], Vector2(oldValue, newValue));
	return true;
}

bool InputManager::FloatListen(int userButton, float oldValue, float newValue)
{
	EventManager::GetSingleton()->call("FloatInputEvent", GetSingleton()->m_InputEventIDNames[userButton], Vector2(oldValue, newValue));
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
