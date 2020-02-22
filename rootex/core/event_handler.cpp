#include "event_handler.h"

EventHandler::EventHandler()
{
}

void EventHandler::handleEvent(const Event* event)
{
	if (!m_Handler)
	{
		ERR("Found event handler with no event handling function. Event handle dropped: " + event->getName());
		return;
	}

	m_Handler(event);
}
