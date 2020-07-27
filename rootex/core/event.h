#pragma once

#include "common/common.h"
#include "entity.h"

/// An Event that is sent out by EventManager.
class Event
{
public:
	/// String defining the type of the event.
	typedef String Type;

private:
	Type m_Type;
	String m_Name;
	Variant m_Data;

public:
	static void RegisterAPI(sol::state& rootex);

	Event(const String& name, const Type type, const Variant data);
	Event(Event&) = delete;
	~Event() = default;
	
	const String& getName() const { return m_Name; };
	const Type& getType() const { return m_Type; };
	/// Returns the payload data sent with an event. Extract typed data after getting the data.
	const Variant& getData() const { return m_Data; }
};
