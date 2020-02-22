#pragma once

#include "common/common.h"
#include "event.h"

#include <functional>
using EventFunction = std::function<void(const Event*)>;

class EventHandler
{
	EventFunction m_Handler;

	friend class Entity;

public:
	EventHandler();
	EventHandler(EventHandler&) = delete;
	~EventHandler() = default;

	void handleEvent(const Event* event);

	const EventFunction& getHandler() const { return m_Handler; }
	void setHandler(EventFunction function) { m_Handler = function; }
};
