#include "common/common.h"

typedef unsigned long EventType;

class Event
{
public:
	Event() {}
	virtual ~Event(void) {}
	virtual String getName() const = 0;
	virtual const EventType getEventType() const = 0;
};

//for testing purposes only
class Example : public Event
{
public:
	static const EventType s_Exp;
	String getName() const override
	{
		return "hello";
	}
	const EventType getEventType(void) const override
	{
		return s_Exp;
	}
};

const EventType Example::s_Exp = 11111;
