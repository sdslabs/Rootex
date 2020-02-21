#pragma once

#include <algorithm>
#include <utility>

#include "common/common.h"
#include "event_handler.h"
#include "event.h"

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

class EventManager
{
	typedef Vector<EventHandler*> EventListenerList;
	typedef Map<Event::Type, EventListenerList> EventListenerMap;
	typedef Vector<Ref<Event>> EventQueue;

	EventListenerMap m_EventListeners;
	EventQueue m_Queues[EVENTMANAGER_NUM_QUEUES];
	unsigned int m_ActiveQueue;

public:
	enum Constant
	{
		Infinite = 0xffffffff
	};

	EventManager();
	~EventManager();
	static EventManager* GetSingleton();

	bool addListener(EventHandler* instance, Event::Type type);
	bool removeListener(EventHandler* instance, Event::Type type);
	bool call(const Ref<Event> event);
	bool deferredCall(const Ref<Event> event);
	bool tick(unsigned long maxMillis = Infinite);
};
