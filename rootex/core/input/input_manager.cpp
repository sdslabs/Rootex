#include "input_manager.h"
#include "event_manager.h"

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
	m_GainputMap.MapBool((gainput::UserButtonId)(int)Event::Type::InputForward, DeviceIDs[Device::Keyboard], KeyboardButton::KeyW);
	m_GainputMap.MapBool((gainput::UserButtonId)(int)Event::Type::InputBackward, DeviceIDs[Device::Keyboard], KeyboardButton::KeyS);
	m_GainputMap.MapBool((gainput::UserButtonId)(int)Event::Type::InputLeft, DeviceIDs[Device::Keyboard], KeyboardButton::KeyA);
	m_GainputMap.MapBool((gainput::UserButtonId)(int)Event::Type::InputRight, DeviceIDs[Device::Keyboard], KeyboardButton::KeyD);
	m_GainputMap.MapBool((gainput::UserButtonId)(int)Event::Type::InputExit, DeviceIDs[Device::Keyboard], KeyboardButton::KeyEscape);

	m_GainputMap.MapFloat((gainput::UserButtonId)(int)Event::Type::InputMouseX, DeviceIDs[Device::Mouse], MouseButton::MouseAxisX);
	m_GainputMap.MapFloat((gainput::UserButtonId)(int)Event::Type::InputMouseY, DeviceIDs[Device::Mouse], MouseButton::MouseAxisY);
}

void InputManager::mapBool(Event::Type action, Device device, DeviceButtonID button)
{
	m_GainputMap.MapBool((gainput::UserButtonId)(int)action, DeviceIDs[device], button);
}

void InputManager::mapFloat(Event::Type action, Device device, DeviceButtonID button)
{
	m_GainputMap.MapFloat((gainput::UserButtonId)(int)action, DeviceIDs[device], button);
}

bool InputManager::isPressed(Event::Type action)
{
	return m_GainputMap.GetBool((gainput::UserButtonId)(int)action);
}

bool InputManager::wasPressed(Event::Type action)
{
	return m_GainputMap.GetBoolWasDown((gainput::UserButtonId)(int)action);
}

float InputManager::getFloat(Event::Type action)
{
	return m_GainputMap.GetFloat((gainput::UserButtonId)(int)action);
}

float InputManager::getDelta(Event::Type action)
{
	return m_GainputMap.GetFloatDelta((gainput::UserButtonId)(int)action);
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
	Event inputEvent("BoolInputEvent", (Event::Type)userButton, Vector2(oldValue, newValue));
	EventManager::GetSingleton()->call(&inputEvent);
	return true;
}

bool InputManager::FloatListen(int userButton, float oldValue, float newValue)
{
	switch ((Event::Type)userButton)
	{
	case Event::Type::InputMouseX:
		InputManager::GetSingleton()->m_MousePositionDelta.x = newValue - oldValue;
		break;
	case Event::Type::InputMouseY:
		InputManager::GetSingleton()->m_MousePositionDelta.y = newValue - oldValue;
		break;
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
