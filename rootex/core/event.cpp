#include "event.h"

Event::Event(const String& name, const Type type, const Variant data)
    : m_Name(name)
    , m_Type(type)
    , m_Data(data)
{
}
