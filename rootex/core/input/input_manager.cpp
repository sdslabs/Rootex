#include "input_manager.h"

void InputManager::initialize(unsigned int width, unsigned int height)
{
	m_GainputManager.SetDisplaySize(width, height);

	DeviceIDs[Device::Mouse] = m_GainputManager.CreateDevice<gainput::InputDeviceMouse>();
	DeviceIDs[Device::Keyboard] = m_GainputManager.CreateDevice<gainput::InputDeviceKeyboard>();
	DeviceIDs[Device::Pad1] = m_GainputManager.CreateDevice<gainput::InputDevicePad>();
	DeviceIDs[Device::Pad2] = m_GainputManager.CreateDevice<gainput::InputDevicePad>();
}

void InputManager::mapBool(InputAction action, Device device, MouseButton button)
{
	m_GainputMap.MapBool((gainput::UserButtonId)(int)action, DeviceIDs[device], button);
}

void InputManager::mapBool(InputAction action, Device device, Key key)
{
	m_GainputMap.MapBool((gainput::UserButtonId)(int)action, DeviceIDs[device], key);
}

void InputManager::mapFloat(InputAction action, Device device, MouseButton button)
{
	m_GainputMap.MapFloat((gainput::UserButtonId)(int)action, DeviceIDs[device], button);
}

void InputManager::mapFloat(InputAction action, Device device, Key key)
{
	m_GainputMap.MapFloat((gainput::UserButtonId)(int)action, DeviceIDs[device], key);
}

bool InputManager::isPressed(InputAction action)
{
	return m_GainputMap.GetBool((gainput::UserButtonId)(int)action);
}

bool InputManager::wasPressed(InputAction action)
{
	return m_GainputMap.GetBoolWasDown((gainput::UserButtonId)(int)action);
}

float InputManager::getFloat(InputAction action)
{
	return m_GainputMap.GetFloat((gainput::UserButtonId)(int)action);
}

float InputManager::getDelta(InputAction action)
{
	return m_GainputMap.GetFloatDelta((gainput::UserButtonId)(int)action);
}

void InputManager::update()
{
	m_GainputManager.Update();
}

InputManager* InputManager::GetSingleton()
{
	static InputManager singleton;
	return &singleton;
}

InputManager::InputManager()
    : m_GainputMap(m_GainputManager)
{
}

InputManager::~InputManager()
{
}

void InputManager::forwardMessage(const MSG& msg)
{
	m_GainputManager.HandleMessage(msg);
}
