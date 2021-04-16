#pragma once

#include "common/common.h"
#include "event.h"

class EventBinderBase
{
public:
	virtual bool hasBinding(const Event::Type& binding) const = 0;
	/// Call only if binding exists
	virtual Variant handle(const Event& event) = 0;
};

template <class T>
class EventBinder : public EventBinderBase
{
	typedef Function<Variant(const Event*)> EventFunction;

	T* m_Self;
	HashMap<Event::Type, EventFunction> m_Bindings;

public:
	EventBinder(T* self)
	    : m_Self(self)
	{
		EventManager::GetSingleton()->addBinder(this);
	}

	~EventBinder()
	{
		EventManager::GetSingleton()->removeBinder(this);
	}

	/// Duplicate bindings will override the previous ones
	void bind(const Event::Type& event, Variant (T::*eventFunction)(const Event*))
	{
		m_Bindings.emplace(event, [this, eventFunction](const Event* e) { return (m_Self->*eventFunction)(e); });
	}

	void bind(const Event::Type& event, const EventFunction& function)
	{
		m_Bindings.emplace(event, function);
	}

	void unbind(const Event::Type& event, EventFunction eventFunction)
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

/// An Event dispatcher and registrar that also allows looking up registered events.
class EventManager
{
	HashMap<EventBinderBase*, bool> m_EventBinders;
	Vector<Function<void()>> m_DeferList;
	Vector<Ref<Event>> m_DeferredCalls;

public:
	static EventManager* GetSingleton();

	/// Defer a singular function till the end of the frame.
	void defer(Function<void()> function);

	/// Add an event binder which binds to several events per object. Does not need to be called externally.
	void addBinder(EventBinderBase* binder);
	void removeBinder(EventBinderBase* binder);

	/// Publish an event. Returns the result of the first event handled.
	Variant returnCall(const Event& event) const;
	Variant returnCall(const Event::Type& eventType, const Variant& data = 0) const;

	void call(const Event& event) const;
	void call(const Event::Type& eventType, const Variant& data = 0) const;

	/// Publish an event that gets evaluated the end of the current frame.
	void deferredCall(Ref<Event> event);
	void deferredCall(const Event::Type& eventType, const Variant& data = 0);

	/// Dispatch deferred events collected so far.
	void dispatchDeferred();

	const HashMap<EventBinderBase*, bool>& getBinders() const { return m_EventBinders; }
};
