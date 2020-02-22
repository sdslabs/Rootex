#pragma once

#include "common/common.h"

class Event
{
public:
	enum class Type;

private:
	Type m_Type;
	String m_Name;
	Variant m_Data;

public:
	Event(const String& name, const Type type, const Variant data);
	Event(Event&) = delete;
	~Event() = default;
	
	const String& getName() const { return m_Name; };
	const Type getEventType() const { return m_Type; };
	const Variant& getData() const { return m_Data; }

public:
	enum class Type
	{
		Test,

		InputStart, // Loop index starter
		InputForward,
		InputRight,
		InputBackward,
		InputLeft,
	    InputTestAction,
		InputJump,
		InputMouseLeftClick,
		InputMouseX,
		InputMouseY,
		InputExit,
		InputEnd // Loop index ender
	};
};
