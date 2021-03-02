
.. _program_listing_file_rootex_core_event.h:

Program Listing for File event.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_event.h>` (``rootex\core\event.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "entity.h"
   
   #define DEFINE_EVENT(eventName, ...) const static inline Event::Type eventName = #eventName
   
   class Event
   {
   public:
       typedef String Type;
   
   private:
       Type m_Type;
       Variant m_Data;
   
   public:
       Event(const Type type, const Variant data);
       Event(Event&) = delete;
       ~Event() = default;
   
       const Type& getType() const { return m_Type; };
       const Variant& getData() const { return m_Data; }
   };
   
   struct RootexEvents
   {
       DEFINE_EVENT(ApplicationExit);
   
       DEFINE_EVENT(DeleteScene, Scene*);
   
       DEFINE_EVENT(OpenedScene);
   
       DEFINE_EVENT(OSPrint, String);
   
       DEFINE_EVENT(UISystemEnableDebugger);
   
       DEFINE_EVENT(UISystemDisableDebugger);
   
       DEFINE_EVENT(WindowGetScreenState);
   
       DEFINE_EVENT(WindowResized, Vector2);
   
       DEFINE_EVENT(WindowToggleFullscreen);
   
       DEFINE_EVENT(QuitWindowRequest);
   
       DEFINE_EVENT(QuitEditorWindow);
   };
