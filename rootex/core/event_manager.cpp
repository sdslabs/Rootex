#include "event_manager.h"

EventManager::EventManager()
{
	m_ActiveQueue = 0;
}

EventManager ::~EventManager() {}

EventManager* EventManager::GetSingleton()
{
	static EventManager singleton;

	return &singleton;
}

//returns true if listener successfully added
bool EventManager::addListener(EventHandler* instance, Event::Type type)
{
	auto&& it = m_EventListeners.find(type);

	if (it == m_EventListeners.end())
	{
		m_EventListeners[type] = { instance };
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

bool EventManager::removeListener(EventHandler* instance, Event::Type type)
{
	bool success = false;
	auto&& it = m_EventListeners.find(type);

	if (it != m_EventListeners.end())
	{
		EventListenerList& listeners = it->second;
		for (auto vecIt = listeners.begin(); vecIt != listeners.end(); ++vecIt)
		{
			if (instance == (*vecIt))
			{
				listeners.erase(vecIt);
				success = true;
				break;
			}
		}
	}

	return success;
}

void EventManager::call(Event* event)
{
	bool processed = false;
	auto&& findIt = m_EventListeners.find(event->getEventType());

	if (findIt != m_EventListeners.end())
	{
		const EventListenerList& eventListenerList = findIt->second;
		for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
		{
			EventHandler* listener = (*it);

			listener->handleEvent(event);
			processed = true;
		}
	}

	if (!processed)
	{
		WARN("Event left unhandled: " + event->getName());
	}
}

void EventManager::deferredCall(const Ref<Event> event)
{
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

bool EventManager::tick(unsigned long maxMillis)
{
	int queueToProcess = m_ActiveQueue;
	m_ActiveQueue = (m_ActiveQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_Queues[m_ActiveQueue].clear();

	while (!m_Queues[queueToProcess].empty())
	{
		Ref<Event> pEvent = m_Queues[queueToProcess].front();
		m_Queues[queueToProcess].erase(m_Queues[queueToProcess].begin());
		const Event::Type eventType = pEvent->getEventType();

		auto findIt = m_EventListeners.find(eventType);
		if (findIt != m_EventListeners.end())
		{
			const EventListenerList& eventListeners = findIt->second;
			for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
			{
				EventHandler* listener = (*it);
				listener->handleEvent(pEvent.get());
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
