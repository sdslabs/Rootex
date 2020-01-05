#pragma once

#include "common/common.h"
#include "event.h"
#include <algorithm>
#include <utility>

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

//GameObject Generic parent class for all game objects
class GameObject
{
public:
	virtual void handleEvent(const Event* event) { OS::PrintLine("event handled"); }
};

class EventManager
{
	typedef Vector<GameObject*> EventListenerList;
	typedef Map<EventType, EventListenerList> EventListenerMap;
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

	bool addListener(GameObject* instance, EventType type);
	bool removeListener(GameObject* instance, EventType type);
	bool call(const Ref<Event> event);
	bool deferredCall(const Ref<Event> event);
	bool tick(unsigned long maxMillis = Infinite);
};
