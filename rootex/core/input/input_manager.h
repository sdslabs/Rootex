#pragma once

#include "common/common.h"
#include "input_action.h"
#include "vendor/Gainput/include/gainput/gainput.h"

typedef gainput::Key Key;
typedef gainput::MouseButton MouseButton;

enum class Device
{
	Mouse,
	Keyboard,
	Pad1,
	Pad2
};

class InputManager
{
	gainput::InputManager m_GainputManager;
	gainput::InputMap m_GainputMap;
	::HashMap<Device, unsigned int> DeviceIDs;

	InputManager();
	InputManager(InputManager&) = delete;
	~InputManager();

	void forwardMessage(const MSG& msg);

	friend class Window;

public:
	static InputManager* GetSingleton();

	void initialize(unsigned int width, unsigned int height);

	void mapBool(InputAction action, Device device, MouseButton button);
	void mapBool(InputAction action, Device device, Key key);
	void mapFloat(InputAction action, Device device, MouseButton button);
	void mapFloat(InputAction action, Device device, Key key);

	bool isPressed(InputAction action);
	bool wasPressed(InputAction action);
	float getFloat(InputAction action);
	float getDelta(InputAction action);

	void update();

	gainput::InputDeviceMouse* getMouse() { return static_cast<gainput::InputDeviceMouse*>(m_GainputManager.GetDevice(DeviceIDs[Device::Mouse])); }
	gainput::InputDeviceKeyboard* getKeyboard() { return static_cast<gainput::InputDeviceKeyboard*>(m_GainputManager.GetDevice(DeviceIDs[Device::Keyboard])); }
	gainput::InputDevicePad* getPad1() {return static_cast<gainput::InputDevicePad*>(m_GainputManager.GetDevice(DeviceIDs[Device::Pad1])); }
	gainput::InputDevicePad* getPad2() { return static_cast<gainput::InputDevicePad*>(m_GainputManager.GetDevice(DeviceIDs[Device::Pad2])); }
};
