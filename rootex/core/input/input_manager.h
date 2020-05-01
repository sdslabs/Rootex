#pragma once

#include "common/common.h"
#include "event.h"
#include "input_listener.h"

#include "vendor/Gainput/include/gainput/gainput.h"

/// Alias for a keyboard device. Allows various keyboard specific operations.
typedef gainput::Key KeyboardButton;
/// Alias for a mouse device. Allows various mouse specific operations.
typedef gainput::MouseButton MouseButton;
/// Alias for a game controller/pad device. Allows various pad specific operations. Gainput (our input library) only supports pads that allow XInput. So non-XInput controllers are not supported.
typedef gainput::PadButton PadButton; 
/// ID of any key, button or analog on the device hardware.
typedef gainput::DeviceButtonId DeviceButtonID;

/// Enumeration of the devices supported. Any new device can be added here.
enum class Device
{
	Mouse,
	Keyboard,
	/// Player 1 pad. Usually the first pad connected.
	Pad1,
	/// Player 2 pad. Usually the second pad connected.
	Pad2
};

struct InputButtonBindingData
{
	enum class Type
	{
		Float,
		Bool
	};
	Type m_Type;
	String m_InputEvent;
	Device m_Device;
	DeviceButtonID m_ButtonID;
};

/// Allows interfacing to game controlling hardware, including mouse, keyboard and XInput controllers.
/// Allows detecting inputs through Event dispatch. 
/// Event data for boolean buttons consists of a Vector2 where Vector2.x and Vector2.y carry the old and new values for the input event respectively.
/// Float buttons should be queried directly by invoking InputManager.
class InputManager
{
	/// Callback from Gainput's internals. Called when a key with bool value is activated.
	static bool BoolListen(int userButton, bool oldValue, bool newValue);
	/// Callback from Gainput's internals. Called when a key with float value is activated.
	static bool FloatListen(int userButton, float oldValue, float newValue);

	gainput::InputManager m_GainputManager;
	gainput::InputMap m_GainputMap;
	InputListener m_Listener;
	bool m_IsEnabled;
	HashMap<Device, unsigned int> DeviceIDs;
	HashMap<String, Vector<InputButtonBindingData>> m_InputSchemes;
	String m_CurrentInputScheme;

	HashMap<unsigned int, Event::Type> m_InputEventIDNames;
	HashMap<Event::Type, unsigned int> m_InputEventNameIDs;

	unsigned int m_Width;
	unsigned int m_Height;

	InputManager();
	InputManager(InputManager&) = delete;
	~InputManager() = default;

	void forwardMessage(const MSG& msg);

	friend class Window;

	unsigned int getNextID();

public:
	static InputManager* GetSingleton();

	void initialize(unsigned int width, unsigned int height);

	void setEnabled(bool enabled);

	void loadSchemes(const JSON::json& inputSchemes);
	void setScheme(const String& schemeName);

	/// Bind an event to a button on a device.
	void mapBool(const Event::Type& action, Device device, DeviceButtonID button);
	/// Bind an event to a float on a device.
	void mapFloat(const Event::Type& action, Device device, DeviceButtonID button);

	void unmap(const Event::Type& action);

	bool isPressed(const Event::Type& action);
	bool wasPressed(const Event::Type& action);
	float getFloat(const Event::Type& action);
	float getFloatDelta(const Event::Type& action);

	void update();

	const gainput::InputMap& getMap() const { return m_GainputMap; }
	gainput::InputDeviceMouse* getMouse() { return static_cast<gainput::InputDeviceMouse*>(m_GainputManager.GetDevice(DeviceIDs[Device::Mouse])); }
	gainput::InputDeviceKeyboard* getKeyboard() { return static_cast<gainput::InputDeviceKeyboard*>(m_GainputManager.GetDevice(DeviceIDs[Device::Keyboard])); }
	gainput::InputDevicePad* getPad1() { return static_cast<gainput::InputDevicePad*>(m_GainputManager.GetDevice(DeviceIDs[Device::Pad1])); }
	gainput::InputDevicePad* getPad2() { return static_cast<gainput::InputDevicePad*>(m_GainputManager.GetDevice(DeviceIDs[Device::Pad2])); }
};
