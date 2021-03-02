
.. _program_listing_file_rootex_core_input_input_listener.h:

Program Listing for File input_listener.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_input_input_listener.h>` (``rootex\core\input\input_listener.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "gainput/gainput.h"
   
   #include <functional>
   typedef std::function<bool(int userButton, bool oldValue, bool newValue)> InputBoolListenerFunction;
   typedef std::function<bool(int userButton, float oldValue, float newValue)> InputFloatListenerFunction;
   
   class InputListener : public gainput::MappedInputListener
   {
       InputBoolListenerFunction m_BoolListenerFunction;
       InputFloatListenerFunction m_FloatListenerFunction;
       int m_ID;
   
   public:
       InputListener(InputBoolListenerFunction boolListener, InputFloatListenerFunction floatListener);
       InputListener(InputListener&) = delete;
       ~InputListener() = default;
   
       virtual bool OnUserButtonBool(gainput::UserButtonId userButton, bool oldValue, bool newValue) override;
       virtual bool OnUserButtonFloat(gainput::UserButtonId userButton, float oldValue, float newValue) override;
   
       int getID() const { return m_ID; }
       void setID(int id) { m_ID = id; }
   };
