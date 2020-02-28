#include "input_manager.h"
#include "event_manager.h"

#include <functional>

const unsigned int Hash(const String& string)
{
	return std::hash<const char*> {}(string.c_str());
}

void InputManager::initialize(unsigned int width, unsigned int height)
{
	m_GainputManager.SetDisplaySize(width, height);
	m_Width = width;
	m_Height = height;

	DeviceIDs[Device::Mouse] = m_GainputManager.CreateDevice<gainput::InputDeviceMouse>();
	DeviceIDs[Device::Keyboard] = m_GainputManager.CreateDevice<gainput::InputDeviceKeyboard>();
	DeviceIDs[Device::Pad1] = m_GainputManager.CreateDevice<gainput::InputDevicePad>();
	DeviceIDs[Device::Pad2] = m_GainputManager.CreateDevice<gainput::InputDevicePad>();

	m_Listener.setID(m_GainputMap.AddListener(&m_Listener));
	enableDefaultContext();
}

void InputManager::enableDefaultContext()
{
	registerInput("InputForward");
	registerInput("InputBackward");
	registerInput("InputLeft");
	registerInput("InputRight");
	registerInput("InputExit");
	registerInput("InputMouseX");
	registerInput("InputMouseY");

	m_GainputMap.MapBool((gainput::UserButtonId)m_InputEventIDs["InputForward"], DeviceIDs[Device::Keyboard], KeyboardButton::KeyW);
	m_GainputMap.MapBool((gainput::UserButtonId)m_InputEventIDs["InputBackward"], DeviceIDs[Device::Keyboard], KeyboardButton::KeyS);
	m_GainputMap.MapBool((gainput::UserButtonId)m_InputEventIDs["InputLeft"], DeviceIDs[Device::Keyboard], KeyboardButton::KeyA);
	m_GainputMap.MapBool((gainput::UserButtonId)m_InputEventIDs["InputRight"], DeviceIDs[Device::Keyboard], KeyboardButton::KeyD);
	m_GainputMap.MapBool((gainput::UserButtonId)m_InputEventIDs["InputExit"], DeviceIDs[Device::Keyboard], KeyboardButton::KeyEscape);

	m_GainputMap.MapFloat((gainput::UserButtonId)m_InputEventIDs["InputMouseX"], DeviceIDs[Device::Mouse], MouseButton::MouseAxisX);
	m_GainputMap.MapFloat((gainput::UserButtonId)m_InputEventIDs["InputMouseY"], DeviceIDs[Device::Mouse], MouseButton::MouseAxisY);
}

void InputManager::registerInput(const String& input)
{
	unsigned int ID = Hash(input);
	m_InputEventIDs[input] = ID;
	m_InputEventNames[ID] = input;
}

void InputManager::mapBool(Event::Type action, Device device, DeviceButtonID button)
{
	m_GainputMap.MapBool((gainput::UserButtonId)m_InputEventIDs[action], DeviceIDs[device], button);
}

void InputManager::mapFloat(Event::Type action, Device device, DeviceButtonID button)
{
	m_GainputMap.MapFloat((gainput::UserButtonId)m_InputEventIDs[action], DeviceIDs[device], button);
}

bool InputManager::isPressed(Event::Type action)
{
	return m_GainputMap.GetBool((gainput::UserButtonId)m_InputEventIDs[action]);
}

bool InputManager::wasPressed(Event::Type action)
{
	return m_GainputMap.GetBoolWasDown((gainput::UserButtonId)m_InputEventIDs[action]);
}

float InputManager::getFloat(Event::Type action)
{
	return m_GainputMap.GetFloat((gainput::UserButtonId)m_InputEventIDs[action]);
}

float InputManager::getDelta(Event::Type action)
{
	return m_GainputMap.GetFloatDelta((gainput::UserButtonId)m_InputEventIDs[action]);
}

void InputManager::update()
{
	m_MousePositionDelta.x = 0.0f;
	m_MousePositionDelta.y = 0.0f;
	m_GainputManager.Update();
}

InputManager* InputManager::GetSingleton()
{
	static InputManager singleton;
	return &singleton;
}

bool InputManager::BoolListen(int userButton, bool oldValue, bool newValue)
{
	EventManager::GetSingleton()->call("BoolInputEvent", GetSingleton()->m_InputEventNames[userButton], Vector2(oldValue, newValue));
	return true;
}

bool InputManager::FloatListen(int userButton, float oldValue, float newValue)
{
	if (userButton == GetSingleton()->m_InputEventIDs["InputMouseX"])
	{
		InputManager::GetSingleton()->m_MousePositionDelta.x = newValue - oldValue;
	}
	else if (userButton == GetSingleton()->m_InputEventIDs["InputMouseY"])
	{
		InputManager::GetSingleton()->m_MousePositionDelta.y = newValue - oldValue;
	}
	return true;
}

InputManager::InputManager()
    : m_GainputMap(m_GainputManager)
    , m_Listener(BoolListen, FloatListen)
    , m_Width(0)
    , m_Height(0)
{
}

InputManager::~InputManager()
{
}

void InputManager::forwardMessage(const MSG& msg)
{
	m_GainputManager.HandleMessage(msg);
}
