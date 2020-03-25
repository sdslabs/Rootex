#pragma once

#include "common/common.h"
#include "event.h"

#include <cstdarg>

#define BIND_EVENT_FUNCTION(stringEventType, function) EventManager::GetSingleton()->addListener(stringEventType, CreateDelegate(function))
#define BIND_EVENT_MEMBER_FUNCTION(stringEventType, classFunction) EventManager::GetSingleton()->addListener(stringEventType, CreateDelegate([this](const Event* event) -> Variant { return this->classFunction(event); }))

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

typedef Ref<Function<Variant(const Event*)>> EventHandlingFunction;

template <class Functor>
EventHandlingFunction CreateDelegate(Functor f)
{
	return EventHandlingFunction(new Function<Variant(const Event*)>(f));
}

class EventManager
{
	typedef Vector<EventHandlingFunction> EventListenerList;
	typedef HashMap<Event::Type, EventListenerList> EventListenerMap;
	typedef Vector<Ref<Event>> EventQueue;

	EventListenerMap m_EventListeners;
	EventQueue m_Queues[EVENTMANAGER_NUM_QUEUES];
	unsigned int m_ActiveQueue;

	EventManager();
	~EventManager();

public:
	static EventManager* GetSingleton();

	enum Constant
	{
		Infinite = 0xffffffff
	};

	bool addEvent(const Event::Type& event);
	void removeEvent(const Event::Type& event);
	bool addListener(const Event::Type& type, EventHandlingFunction instance);
	bool removeListener(const EventHandlingFunction handlerName, const Event::Type& type);

	Variant returnCall(const String& eventName, const Event::Type& eventType, const Variant& data);
	void call(const String& eventName, const Event::Type& eventType, const Variant& data);
	void deferredCall(const String& eventName, const Event::Type& eventType, const Variant& data);
	bool dispatchDeferred(unsigned long maxMillis = Infinite);

	const EventListenerMap& getRegisteredEvents() const { return m_EventListeners; }
};
