#pragma once

#include "common/common.h"
typedef unsigned long EventType;

class Event
{
public:
	Event() {}
	virtual ~Event() {}
	virtual String getName() const = 0;
	virtual const EventType getEventType() const = 0;
};

//for testing purposes only
class Example : public Event
{
public:
	static const EventType s_ExampleID = 0;
	String getName() const override
	{
		return "hello";
	}
	const EventType getEventType() const override
	{
		return s_ExampleID;
	}
};
