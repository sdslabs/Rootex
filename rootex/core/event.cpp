#include "event.h"

void Event::RegisterAPI(sol::state& rootex)
{
	sol::usertype<Event> event = rootex.new_usertype<Event>("Event", sol::constructors<Event(const String&, const Event::Type, const Variant)>());
	event["getName"] = &Event::getName;
	event["getType"] = &Event::getType;
	event["getData"] = &Event::getData;
}

Event::Event(const String& name, const Type type, const Variant data)
    : m_Name(name)
    , m_Type(type)
    , m_Data(data)
{
}
