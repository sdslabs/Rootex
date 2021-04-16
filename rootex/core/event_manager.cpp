#include "event_manager.h"

#include "entity.h"
#include "scene.h"

EventManager* EventManager::GetSingleton()
{
	static EventManager singleton;
	return &singleton;
}

void EventManager::defer(Function<void()> function)
{
	m_DeferList.push_back(function);
}

void EventManager::addBinder(EventBinderBase* binder)
{
	if (m_EventBinders.find(binder) == m_EventBinders.end())
	{
		m_EventBinders[binder] = true;
	}
}

void EventManager::removeBinder(EventBinderBase* binder)
{
	if (m_EventBinders.find(binder) != m_EventBinders.end())
	{
		m_EventBinders.erase(binder);
	}
}

Variant EventManager::returnCall(const Event& event) const
{
	for (auto& [binder, value] : m_EventBinders)
	{
		if (binder->hasBinding(event.getType()))
		{
			return binder->handle(event);
		}
	}
	return false;
}

void EventManager::call(const Event& event) const
{
	for (auto& [binder, value] : m_EventBinders)
	{
		if (binder->hasBinding(event.getType()))
		{
			binder->handle(event);
		}
	}
}

Variant EventManager::returnCall(const Event::Type& eventType, const Variant& data) const
{
	Event event(eventType, data);
	return returnCall(event);
}

void EventManager::call(const Event::Type& eventType, const Variant& data) const
{
	Event event(eventType, data);
	call(event);
}

void EventManager::deferredCall(Ref<Event> event)
{
	m_DeferredCalls.push_back(event);
}

void EventManager::deferredCall(const Event::Type& eventType, const Variant& data)
{
	Ref<Event> event(new Event(eventType, data));
	deferredCall(event);
}

void EventManager::dispatchDeferred()
{
	for (auto& function : m_DeferList)
	{
		function();
	}
	m_DeferList.clear();

	for (auto& deferredEvent : m_DeferredCalls)
	{
		call(*deferredEvent);
	}
	m_DeferredCalls.clear();
}
