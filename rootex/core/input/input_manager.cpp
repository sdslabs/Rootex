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

void InputManager::buildBindings()
{
	m_InputEventIDNames.clear();
	m_InputEventNameIDs.clear();
	m_GainputMap.Clear();

	// Condense the scheme stack to reduce redundant work
	Vector<String> newStack;
	for (auto& schemeName = m_CurrentSchemeStack.rbegin(); schemeName != m_CurrentSchemeStack.rend(); schemeName++)
	{
		if (std::find(newStack.begin(), newStack.end(), *schemeName) == newStack.end())
		{
			newStack.push_back(*schemeName);
		}
	}
	m_CurrentSchemeStack = Vector<String>(newStack.rbegin(), newStack.rend());

	for (auto& scheme : m_CurrentSchemeStack)
	{
		for (auto& boolBinding : m_InputSchemes.at(scheme).bools)
		{
			mapBool(boolBinding.inputEvent, boolBinding.device, boolBinding.button);
		}
		for (auto& floatBinding : m_InputSchemes.at(scheme).floats)
		{
			mapFloat(floatBinding.inputEvent, floatBinding.device, floatBinding.button);
		}
	}
}

void InputManager::setEnabled(bool enabled)
{
	if (enabled)
	{
		PRINT("Input enabled");
		m_GainputMap.AddListener(&m_Listener);
		buildBindings();
	}
	else
	{
		PRINT("Input disabled");
		m_GainputMap.Clear();
		m_GainputManager.RemoveListener(m_Listener.getID());
	}
	m_IsEnabled = enabled;
}

void InputManager::loadSchemes(const HashMap<String, InputScheme>& inputSchemes)
{
	int count = m_InputSchemes.size();
	for (auto& [name, scheme] : inputSchemes)
	{
		addScheme(name, scheme);
	}
	PRINT("Registered " + std::to_string(inputSchemes.size()) + " more input schemes on top of " + std::to_string(count) + " existing ones");
}

void InputManager::addScheme(const String& name, const InputScheme& inputScheme)
{
	if (m_InputSchemes.find(name) != m_InputSchemes.end())
	{
		WARN("Overriding input scheme: " + name);
	}
	m_InputSchemes[name] = inputScheme;
}

void InputManager::popScheme()
{
	if (m_CurrentSchemeStack.empty())
	{
		WARN("Input scheme stack is empty. We don't pop anymore.");
		return;
	}

	String toPop = m_CurrentSchemeStack.back();
	m_CurrentSchemeStack.pop_back();
	buildBindings();

	PRINT(toPop + " input scheme was popped");
}

void InputManager::flushSchemes()
{
	m_InputSchemes.clear();
	m_CurrentSchemeStack.clear();
	buildBindings();
}

void InputManager::pushScheme(const String& name)
{
	if (m_InputSchemes.find(name) == m_InputSchemes.end())
	{
		if (!name.empty())
		{
			WARN("Could not find a registered input scheme of name: " + name);
		}
		return;
	}

	m_CurrentSchemeStack.push_back(name);
	buildBindings();
	PRINT("Pushed input scheme: " + name);
}

void InputManager::mapBool(const Event::Type& action, Device device, DeviceButtonID button)
{
	m_InputEventNameIDs[action] = getNextID((int)device, (int)button);
	m_InputEventIDNames[m_InputEventNameIDs[action]] = action;
	if (!m_GainputMap.MapBool((gainput::UserButtonId)m_InputEventNameIDs[action], DeviceIDs[device], button))
	{
		WARN("Bool mapping could not done: " + action);
	}
}

void InputManager::mapFloat(const Event::Type& action, Device device, DeviceButtonID button)
{
	m_InputEventNameIDs[action] = getNextID((int)device, (int)button);
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
	return m_IsEnabled && m_GainputMap.GetBool((gainput::UserButtonId)m_InputEventNameIDs.at(action));
}

bool InputManager::wasPressed(const Event::Type& action)
{
	return m_IsEnabled && m_GainputMap.GetBoolWasDown((gainput::UserButtonId)m_InputEventNameIDs.at(action));
}

bool InputManager::hasPressed(const Event::Type& action)
{
	return m_IsEnabled && m_GainputMap.GetBoolIsNew((gainput::UserButtonId)m_InputEventNameIDs.at(action));
}

float InputManager::getFloat(const Event::Type& action)
{
	if (m_IsEnabled)
	{
		return m_GainputMap.GetFloat((gainput::UserButtonId)m_InputEventNameIDs.at(action));
	}
	return 0;
}

float InputManager::getFloatDelta(const Event::Type& action)
{
	if (m_IsEnabled)
	{
		return m_GainputMap.GetFloatDelta((gainput::UserButtonId)m_InputEventNameIDs.at(action));
	}
	return 0;
}

Vector2 InputManager::getMousePosition()
{
	return { getMouse()->GetFloat(MouseButton::MouseAxisX), getMouse()->GetFloat(MouseButton::MouseAxisY) };
}

Array<String, 23> InputManager::getMouseButtonName()
{
	Array<String, 23> mouseKeys = {
		"mouse_left",
		"mouse_middle",
		"mouse_right",
		"mouse_3",
		"mouse_4",
		"mouse_5",
		"mouse_6",
		"mouse_7",
		"mouse_8",
		"mouse_9",
		"mouse_10",
		"mouse_11",
		"mouse_12",
		"mouse_13",
		"mouse_14",
		"mouse_15",
		"mouse_16",
		"mouse_17",
		"mouse_18",
		"mouse_19",
		"mouse_20",
		"mouse_x",
		"mouse_y"
	};
	return mouseKeys;
}

Array<String, 166> InputManager::getKeyboardButtonName()
{
	Array<String, 166> keyboardKeys = {
		"escape",
		"f1",
		"f2",
		"f3",
		"f4",
		"f5",
		"f6",
		"f7",
		"f8",
		"f9",
		"f10",
		"f11",
		"f12",
		"print",
		"scroll_lock",
		"break",
		"space",
		"apostrophe",
		"comma",
		"minus",
		"period",
		"slash",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"semicolon",
		"less",
		"equal",
		"a",
		"b",
		"c",
		"d",
		"e",
		"f",
		"g",
		"h",
		"i",
		"j",
		"k",
		"l",
		"m",
		"n",
		"o",
		"p",
		"q",
		"r",
		"s",
		"t",
		"u",
		"v",
		"w",
		"x",
		"y",
		"z",
		"bracket_left",
		"backslash",
		"bracket_right",
		"grave",
		"left",
		"right",
		"up",
		"down",
		"insert",
		"home",
		"delete",
		"end",
		"page_up",
		"page_down",
		"num_lock",
		"kp_divide",
		"kp_multiply",
		"kp_subtract",
		"kp_add",
		"kp_enter",
		"kp_insert",
		"kp_end",
		"kp_down",
		"kp_page_down",
		"kp_left",
		"kp_begin",
		"kp_right",
		"kp_home",
		"kp_up",
		"kp_page_up",
		"kp_delete",
		"back_space",
		"tab",
		"return",
		"caps_lock",
		"shift_l",
		"ctrl_l",
		"super_l",
		"alt_l",
		"alt_r",
		"super_r",
		"menu",
		"ctrl_r",
		"shift_r",
		"back",
		"soft_left",
		"soft_right",
		"call",
		"endcall",
		"star",
		"pound",
		"dpad_center",
		"volume_up",
		"volume_down",
		"power",
		"camera",
		"clear",
		"symbol",
		"explorer",
		"envelope",
		"equals",
		"at",
		"headsethook",
		"focus",
		"plus",
		"notification",
		"search",
		"media_play_pause",
		"media_stop",
		"media_next",
		"media_previous",
		"media_rewind",
		"media_fast_forward",
		"mute",
		"pictsymbols",
		"switch_charset",
		"forward",
		"extra_1",
		"extra_2",
		"extra_3",
		"extra_4",
		"extra_5",
		"extra_6",
		"fn",
		"circumflex",
		"ssharp",
		"acute",
		"alt_gr",
		"numbersign",
		"udiaeresis",
		"adiaeresis",
		"odiaeresis",
		"section",
		"aring",
		"diaeresis",
		"twosuperior",
		"right_parenthesis",
		"dollar",
		"ugrave",
		"asterisk",
		"colon",
		"exclam",
		"brace_left",
		"brace_right",
		"sys_rq"
	};
	return keyboardKeys;
}

Array<String, 20> InputManager::getPadButtonName()
{
	Array<String, 20> padKeys = {
		"pad_left_stick_x",
		"pad_left_stick_y",
		"pad_right_stick_x",
		"pad_right_stick_y",
		"pad_button_start",
		"pad_button_select",
		"pad_button_left",
		"pad_button_right",
		"pad_button_up",
		"pad_button_down",
		"pad_button_a",
		"pad_button_b",
		"pad_button_x",
		"pad_button_y",
		"pad_button_l1",
		"pad_button_r1",
		"pad_button_l2",
		"pad_button_r2",
		"pad_button_l3",
		"pad_button_r3"
	};
	return padKeys;
}

void InputManager::update()
{
	m_GainputManager.Update();
}

void InputManager::setDisplaySize(const Vector2& newSize)
{
	m_GainputManager.SetDisplaySize(newSize.x, newSize.y);
}

unsigned int InputManager::getNextID(int device, int button)
{
	static unsigned int count = 0;
	static HashMap<int, HashMap<int, int>> cache;

	if (cache.find(device) != cache.end())
	{
		if (cache.at(device).find(button) != cache.at(device).end())
		{
			return cache.at(device).at(button);
		}
	}

	cache[device][button] = count;
	return count++;
}

InputManager* InputManager::GetSingleton()
{
	static InputManager singleton;
	return &singleton;
}

bool InputManager::BoolListen(int userButton, bool oldValue, bool newValue)
{
	EventManager::GetSingleton()->call(GetSingleton()->m_InputEventIDNames.at(userButton), Vector2(oldValue, newValue));
	return true;
}

bool InputManager::FloatListen(int userButton, float oldValue, float newValue)
{
	EventManager::GetSingleton()->call(GetSingleton()->m_InputEventIDNames.at(userButton), Vector2(oldValue, newValue));
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
	j["active"] = s.isActive;
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
	s.isActive = j.value("active", false);
}
