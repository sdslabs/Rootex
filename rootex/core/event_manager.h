#pragma once

#include "common/common.h"
#include "event.h"

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

typedef Ref<Function<void(const Event*)>> EventHandlingFunction;

template <class Functor>
EventHandlingFunction CreateDelegate(Functor f)
{
	return EventHandlingFunction(new Function<void(const Event*)>(f));
}

class EventManager
{
	typedef Vector<EventHandlingFunction> EventListenerList;
	typedef Map<Event::Type, EventListenerList> EventListenerMap;
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

	bool addListener(Event::Type type, EventHandlingFunction instance);
	bool removeListener(const EventHandlingFunction handlerName, Event::Type type);

	void call(Event* event);
	void deferredCall(const Ref<Event> event);
	bool dispatchDeferred(unsigned long maxMillis = Infinite);
};
