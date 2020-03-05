#pragma once

#include "common/common.h"
#include "event.h"
#include "input_listener.h"

#include "vendor/Gainput/include/gainput/gainput.h"

#define MAX_BUTTON_BINDINGS 10

class InputListener;

typedef gainput::Key KeyboardButton;
typedef gainput::MouseButton MouseButton;
typedef gainput::PadButton PadButton;
typedef gainput::DeviceButtonId DeviceButtonID;

enum class Device
{
	Mouse,
	Keyboard,
	Pad1,
	Pad2
};

class InputManager
{
	static bool BoolListen(int userButton, bool oldValue, bool newValue);
	static bool FloatListen(int userButton, float oldValue, float newValue);

	gainput::InputManager m_GainputManager;
	gainput::InputMap m_GainputMap;
	InputListener m_Listener;
	HashMap<Device, unsigned int> DeviceIDs;

	HashMap<unsigned int, Event::Type> m_InputEventNames;
	HashMap<Event::Type, unsigned int> m_InputEventIDs;

	unsigned int m_Width;
	unsigned int m_Height;
	Vector2 m_MousePositionDelta;

	InputManager();
	InputManager(InputManager&) = delete;
	~InputManager();

	void registerInput(const String& input);
	void forwardMessage(const MSG& msg);

	friend class Window;

public:
	static InputManager* GetSingleton();

	void initialize(unsigned int width, unsigned int height);

	void enableDefaultContext();

	void mapBool(Event::Type action, Device device, DeviceButtonID button);
	void mapFloat(Event::Type action, Device device, DeviceButtonID button);

	bool isPressed(Event::Type action);
	bool wasPressed(Event::Type action);
	float getFloat(Event::Type action);
	float getDelta(Event::Type action);

	const Vector2& getMousePositionDelta() const { return m_MousePositionDelta; }

	void update();

	gainput::InputDeviceMouse* getMouse() { return static_cast<gainput::InputDeviceMouse*>(m_GainputManager.GetDevice(DeviceIDs[Device::Mouse])); }
	gainput::InputDeviceKeyboard* getKeyboard() { return static_cast<gainput::InputDeviceKeyboard*>(m_GainputManager.GetDevice(DeviceIDs[Device::Keyboard])); }
	gainput::InputDevicePad* getPad1() { return static_cast<gainput::InputDevicePad*>(m_GainputManager.GetDevice(DeviceIDs[Device::Pad1])); }
	gainput::InputDevicePad* getPad2() { return static_cast<gainput::InputDevicePad*>(m_GainputManager.GetDevice(DeviceIDs[Device::Pad2])); }
};
