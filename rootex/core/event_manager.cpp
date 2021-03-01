#include "event_manager.h"

#include "entity.h"
#include "scene.h"

EventManager::EventManager()
{
	m_ActiveQueue = 0;
}

EventManager* EventManager::GetSingleton()
{
	static EventManager singleton;
	return &singleton;
}

void EventManager::defer(Function<void()> function)
{
	m_DeferList.push_back(function);
}

bool EventManager::addEvent(const Event::Type& event)
{
	if (m_EventListeners.find(event) == m_EventListeners.end())
	{
		m_EventListeners[event] = {};
		return true;
	}
	return false;
}

void EventManager::removeEvent(const Event::Type& event)
{
	m_EventListeners.erase(event);
}

Variant EventManager::returnCall(const Event& event)
{
	auto&& findIt = m_EventListeners.find(event.getType());

	if (findIt != m_EventListeners.end())
	{
		return findIt->second.front()(&event);
	}
	return false;
}

Variant EventManager::returnCall(const Event::Type& eventType, const Variant& data)
{
	Event event(eventType, data);
	return returnCall(event);
}

void EventManager::call(const Event& event)
{
	bool processed = false;
	auto&& findIt = m_EventListeners.find(event.getType());

	if (findIt != m_EventListeners.end())
	{
		Vector<EventFunction>& eventListenerList = findIt->second;
		int toDelete = -1;
		for (int i = 0; i != eventListenerList.size(); i++)
		{
			EventFunction& listener = eventListenerList[i];
			if (listener)
			{
				listener(&event);
			}
			else
			{
				toDelete = i;
			}
			processed = true;
		}

		if (toDelete != -1)
		{
			eventListenerList.erase(eventListenerList.begin() + toDelete);
		}
	}
}

void EventManager::call(const Event::Type& eventType, const Variant& data)
{
	Event event(eventType, data);
	call(event);
}

void EventManager::deferredCall(Ref<Event> event)
{
	if (!(m_ActiveQueue >= 0 && m_ActiveQueue < EVENTMANAGER_NUM_QUEUES))
	{
		WARN("Event left unhandled: " + event->getType());
	}

	auto&& findIt = m_EventListeners.find(event->getType());
	if (findIt != m_EventListeners.end())
	{
		m_Queues[m_ActiveQueue].push_back(event);
		return;
	}
	else
	{
		WARN("Event left unhandled: " + event->getType());
	}
}

void EventManager::deferredCall(const Event::Type& eventType, const Variant& data)
{
	Ref<Event> event(new Event(eventType, data));
	deferredCall(event);
}

bool EventManager::dispatchDeferred(unsigned long maxMillis)
{
	for (auto& function : m_DeferList)
	{
		function();
	}
	m_DeferList.clear();

	int queueToProcess = m_ActiveQueue;
	m_ActiveQueue = (m_ActiveQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_Queues[m_ActiveQueue].clear();

	while (!m_Queues[queueToProcess].empty())
	{
		Ref<Event> event = m_Queues[queueToProcess].front();
		m_Queues[queueToProcess].erase(m_Queues[queueToProcess].begin());
		const Event::Type eventType = event->getType();

		auto findIt = m_EventListeners.find(eventType);
		if (findIt != m_EventListeners.end())
		{
			const Vector<EventFunction>& eventListeners = findIt->second;
			for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
			{
				EventFunction listener = *it;
				listener(event.get());
			}
		}

		// check to see if time ran out
		// currMs = GetTickCount();
		// if (maxMillis != IEventManager::kINFINITE && currMs >= maxMs)
		// {
		//  	WARN("Aborting event processing; time ran out");
		//  	break;
		// }
	}
	// If we couldn't process all of the events, push the remaining events to
	// the new active queue.
	bool queueFlushed = (m_Queues[queueToProcess].empty());
	if (!queueFlushed)
	{
		while (!m_Queues[queueToProcess].empty())
		{
			Ref<Event> pEvent = m_Queues[queueToProcess].back();
			m_Queues[queueToProcess].pop_back();
			m_Queues[m_ActiveQueue].insert(m_Queues[m_ActiveQueue].begin(), pEvent);
		}
	}
	return queueFlushed;
}

void EventManager::releaseAllEventListeners()
{
	m_EventListeners.clear();
}

bool EventManager::addListener(const Event::Type& type, EventFunction instance)
{
	m_EventListeners[type].push_back(instance);
	return true;
}
