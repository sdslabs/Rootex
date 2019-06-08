#pragma once

class Event
{
private:

	enum Type
	{
		User
	};

	const Type m_Type;

public:
	
	Event() = default;
	Event(Type TypeName);
	virtual ~Event() = default;
	
	const Type &GetType() const { return m_Type; }

protected:

};

class EventDispatcher
{
private:

public:

protected:

};
