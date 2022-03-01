
.. _program_listing_file_rootex_core_input_input_manager.h:

Program Listing for File input_manager.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_input_input_manager.h>` (``rootex\core\input\input_manager.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "event.h"
   #include "input_listener.h"
   
   #include "vendor/Gainput/include/gainput/gainput.h"
   
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
   
   struct InputDescription
   {
       Device device;
       DeviceButtonID button;
       Event::Type inputEvent;
   };
   
   void to_json(JSON::json& j, const InputDescription& s);
   void from_json(const JSON::json& j, InputDescription& s);
   
   struct InputScheme
   {
       Vector<InputDescription> bools;
       Vector<InputDescription> floats;
   };
   
   void to_json(JSON::json& j, const InputScheme& s);
   void from_json(const JSON::json& j, InputScheme& s);
   
   class InputManager
   {
       static bool BoolListen(int userButton, bool oldValue, bool newValue);
       static bool FloatListen(int userButton, float oldValue, float newValue);
   
       gainput::InputManager m_GainputManager;
       gainput::InputMap m_GainputMap;
       InputListener m_Listener;
       bool m_IsEnabled;
       HashMap<Device, unsigned int> DeviceIDs;
       HashMap<String, InputScheme> m_InputSchemes;
       Vector<String> m_CurrentSchemeStack;
   
       HashMap<unsigned int, Event::Type> m_InputEventIDNames;
       HashMap<Event::Type, unsigned int> m_InputEventNameIDs;
   
       unsigned int m_Width;
       unsigned int m_Height;
   
       InputManager();
       InputManager(InputManager&) = delete;
       ~InputManager() = default;
   
       void forwardMessage(const MSG& msg);
   
       friend class Window;
   
       unsigned int getNextID(int device, int button);
       void buildBindings();
   
   public:
       static InputManager* GetSingleton();
       static void SetEnabled(bool enabled) { GetSingleton()->setEnabled(enabled); };
       static void MapBool(const Event::Type& action, Device device, DeviceButtonID button) { GetSingleton()->mapBool(action, device, button); };
       static void MapFloat(const Event::Type& action, Device device, DeviceButtonID button) { GetSingleton()->mapBool(action, device, button); };
       static bool IsPressed(const Event::Type& action) { return GetSingleton()->isPressed(action); };
       static bool HasPressed(const Event::Type& action) { return GetSingleton()->hasPressed(action); };
       static bool WasPressed(const Event::Type& action) { return GetSingleton()->wasPressed(action); };
       static float GetFloat(const Event::Type& action) { return GetSingleton()->getFloat(action); };
       static float GetFloatDelta(const Event::Type& action) { return GetSingleton()->getFloatDelta(action); };
       static void Unmap(const Event::Type& action) { GetSingleton()->unmap(action); };
       static Vector2 GetMousePosition() { return GetSingleton()->getMousePosition(); };
   
       void initialize(unsigned int width, unsigned int height);
   
       void setEnabled(bool enabled);
   
       void loadSchemes(const HashMap<String, InputScheme>& inputSchemes);
       void addScheme(const String& name, const InputScheme& inputScheme);
       void pushScheme(const String& schemeName);
       void popScheme();
       void flushSchemes();
   
       void mapBool(const Event::Type& action, Device device, DeviceButtonID button);
       void mapFloat(const Event::Type& action, Device device, DeviceButtonID button);
   
       void unmap(const Event::Type& action);
   
       bool isPressed(const Event::Type& action);
       bool hasPressed(const Event::Type& action);
       bool wasPressed(const Event::Type& action);
       float getFloat(const Event::Type& action);
       float getFloatDelta(const Event::Type& action);
   
       Vector2 getMousePosition();
   
       void update();
       void setDisplaySize(const Vector2& newSize);
   
       const gainput::InputMap& getMap() const { return m_GainputMap; }
       gainput::InputDeviceMouse* getMouse() { return static_cast<gainput::InputDeviceMouse*>(m_GainputManager.GetDevice(DeviceIDs[Device::Mouse])); }
       gainput::InputDeviceKeyboard* getKeyboard() { return static_cast<gainput::InputDeviceKeyboard*>(m_GainputManager.GetDevice(DeviceIDs[Device::Keyboard])); }
       gainput::InputDevicePad* getPad1() { return static_cast<gainput::InputDevicePad*>(m_GainputManager.GetDevice(DeviceIDs[Device::Pad1])); }
       gainput::InputDevicePad* getPad2() { return static_cast<gainput::InputDevicePad*>(m_GainputManager.GetDevice(DeviceIDs[Device::Pad2])); }
   };
