
.. _program_listing_file_rootex_core_event_manager.h:

Program Listing for File event_manager.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_event_manager.h>` (``rootex\core\event_manager.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "event.h"
   
   #define BIND_EVENT_FUNCTION(stringEventType, function) EventManager::GetSingleton()->addListener(stringEventType, function)
   #define BIND_EVENT_MEMBER_FUNCTION(stringEventType, classFunction) EventManager::GetSingleton()->addListener(stringEventType, [this](const Event* event) -> Variant { return this->classFunction(event); })
   
   const unsigned int EVENTMANAGER_NUM_QUEUES = 2;
   
   typedef Function<Variant(const Event*)> EventFunction;
   
   class EventManager
   {
       Vector<Function<void()>> m_DeferList;
       HashMap<Event::Type, Vector<EventFunction>> m_EventListeners;
       Vector<Ref<Event>> m_Queues[EVENTMANAGER_NUM_QUEUES];
       unsigned int m_ActiveQueue;
   
       EventManager();
       ~EventManager() = default;
   
   public:
       static EventManager* GetSingleton();
   
       enum Constant
       {
           Infinite = 0xffffffff
       };
   
       void defer(Function<void()> function);
       bool addEvent(const Event::Type& event);
       void removeEvent(const Event::Type& event);
       bool addListener(const Event::Type& type, EventFunction instance);
       Variant returnCall(const Event& event);
       Variant returnCall(const Event::Type& eventType, const Variant& data = 0);
       void call(const Event& event);
       void call(const Event::Type& eventType, const Variant& data = 0);
       void deferredCall(Ref<Event> event);
       void deferredCall(const Event::Type& eventType, const Variant& data = 0);
       bool dispatchDeferred(unsigned long maxMillis = Infinite);
   
       void releaseAllEventListeners();
   
       const HashMap<Event::Type, Vector<EventFunction>>& getRegisteredEvents() const { return m_EventListeners; }
   };
