#include "event_manager.h"

EventManager::EventManager()
{
	m_ActiveQueue = 0;
}

EventManager ::~EventManager() 
{
}

EventManager* EventManager::GetSingleton()
{
	static EventManager singleton;
	return &singleton;
}

Variant EventManager::returnCall(const String& eventName, const Event::Type& eventType, const Variant& data)
{
	bool processed = false;
	Event event(eventName, eventType, data);
	auto&& findIt = m_EventListeners.find(event.getEventType());

	if (findIt != m_EventListeners.end())
	{
		return (*findIt->second.front().get())(&event);
	}
	return false;
}

void EventManager::call(const String& eventName, const Event::Type& eventType, const Variant& data)
{
	bool processed = false;
	Event event(eventName, eventType, data);
	auto&& findIt = m_EventListeners.find(event.getEventType());

	if (findIt != m_EventListeners.end())
	{
		const EventListenerList& eventListenerList = findIt->second;
		for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
		{
			EventHandlingFunction listener = *it;
 			(*listener.get())(&event);
			processed = true;
		}
	}
}

void EventManager::deferredCall(const String& eventName, const Event::Type& eventType, const Variant& data)
{
	Ref<Event> event(new Event(eventName, eventType, data));

	if (!(m_ActiveQueue >= 0 && m_ActiveQueue < EVENTMANAGER_NUM_QUEUES))
	{
		WARN("Event left unhandled: " + event->getName());
	}

	auto&& findIt = m_EventListeners.find(event->getEventType());
	if (findIt != m_EventListeners.end())
	{
		m_Queues[m_ActiveQueue].push_back(event);
		return;
	}
	else
	{
		WARN("Event left unhandled: " + event->getName());
	}
}

bool EventManager::dispatchDeferred(unsigned long maxMillis)
{
	int queueToProcess = m_ActiveQueue;
	m_ActiveQueue = (m_ActiveQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_Queues[m_ActiveQueue].clear();

	while (!m_Queues[queueToProcess].empty())
	{
		Ref<Event> event = m_Queues[queueToProcess].front();
		m_Queues[queueToProcess].erase(m_Queues[queueToProcess].begin());
		const Event::Type eventType = event->getEventType();

		auto findIt = m_EventListeners.find(eventType);
		if (findIt != m_EventListeners.end())
		{
			const EventListenerList& eventListeners = findIt->second;
			for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
			{
				EventHandlingFunction listener = *it;
				(*listener.get())(event.get());
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

bool EventManager::addListener(const Event::Type& type, EventHandlingFunction instance)
{
	auto&& it = m_EventListeners.find(type);
	if (it == m_EventListeners.end())
	{
		m_EventListeners[type].push_back(instance);
		return true;
	}

	if (std::find(it->second.begin(), it->second.end(), instance) != it->second.end())
	{
		return false;
	}
	else
	{
		it->second.push_back(instance);
		return true;
	}
}

bool EventManager::removeListener(const EventHandlingFunction handlerFunction, const Event::Type& type)
{
	bool success = false;
	auto&& it = m_EventListeners.find(type);

	if (it != m_EventListeners.end())
	{
		EventListenerList& listeners = it->second;
		for (auto& registeredHandler = listeners.begin(); registeredHandler != listeners.end(); ++registeredHandler)
		{
			if (handlerFunction == *registeredHandler)
			{
				listeners.erase(registeredHandler);
				success = true;
				break;
			}
		}
	}

	return success;
}
