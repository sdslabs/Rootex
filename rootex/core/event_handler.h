#pragma once

#include "common/common.h"
#include "event.h"

#include <functional>
using EventFunction = std::function<void(const Event*)>;

class EventHandler
{
	EventFunction m_Handler;

public:
	EventHandler(const EventFunction& handler);
	EventHandler(EventHandler&) = delete;
	~EventHandler() = default;

	void handleEvent(const Event* event) { m_Handler(event); }
};
