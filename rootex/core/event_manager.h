#pragma once

#include "common.h"
#include "event.h"
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

//GameObject Generic parent class for all game objects
class GameObject
{
public:
	virtual void handleEvent(const Event* event) {}
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
	enum CONSTANT
	{
		INFINITE = 0xffffffff
	};

	EventManager();
	~EventManager();

	bool addListener(GameObject* instance, EventType type);
	bool removeListener(GameObject* instance, EventType type);
	bool call(const Ref<Event> event);
	bool deferredCall(const Ref<Event> event);
	bool tick(unsigned long maxMillis);
};
