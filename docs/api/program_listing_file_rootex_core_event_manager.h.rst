
.. _program_listing_file_rootex_core_event_manager.h:

Program Listing for File event_manager.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_event_manager.h>` (``rootex\core\event_manager.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "event.h"
   
   class EventBinderBase
   {
   public:
       virtual bool hasBinding(const Event::Type& binding) const = 0;
       virtual Variant handle(const Event& event) = 0;
   };
   
   template <class T>
   class EventBinder : public EventBinderBase
   {
       typedef Function<Variant(const Event*)> EventFunction;
   
       HashMap<Event::Type, EventFunction> m_Bindings;
   
   public:
       EventBinder()
       {
           EventManager::GetSingleton()->addBinder(this);
       }
   
       ~EventBinder()
       {
           EventManager::GetSingleton()->removeBinder(this);
       }
   
       void bind(const Event::Type& event, T* self, Variant (T::*eventFunction)(const Event*))
       {
           m_Bindings.emplace(event, [self, eventFunction](const Event* e) { return (self->*eventFunction)(e); });
       }
   
       void bind(const Event::Type& event, EventFunction function)
       {
           m_Bindings.emplace(event, function);
       }
   
       void unbind(const Event::Type& event)
       {
           m_Bindings.erase(event);
       }
   
       void unbindAll()
       {
           m_Bindings.clear();
       }
   
       bool hasBinding(const Event::Type& binding) const override
       {
           return m_Bindings.find(binding) != m_Bindings.end();
       }
   
       Variant handle(const Event& event) override
       {
           return m_Bindings.at(event.getType())(&event);
       }
   };
   
   class EventManager
   {
       HashMap<EventBinderBase*, bool> m_EventBinders;
       Vector<Function<void()>> m_DeferList;
       Vector<Ref<Event>> m_DeferredCalls;
   
   public:
       static EventManager* GetSingleton();
   
       void defer(Function<void()> function);
   
       void addBinder(EventBinderBase* binder);
       void removeBinder(EventBinderBase* binder);
   
       Variant returnCall(const Event& event) const;
       Variant returnCall(const Event::Type& eventType, const Variant& data = 0) const;
   
       void call(const Event& event) const;
       void call(const Event::Type& eventType, const Variant& data = 0) const;
   
       void deferredCall(Ref<Event> event);
       void deferredCall(const Event::Type& eventType, const Variant& data = 0);
   
       void dispatchDeferred();
   
       const HashMap<EventBinderBase*, bool>& getBinders() const { return m_EventBinders; }
   };
