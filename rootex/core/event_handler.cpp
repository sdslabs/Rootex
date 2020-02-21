#include "event_handler.h"

EventHandler::EventHandler(const EventFunction& handler)
    : m_Handler(handler)
{
}
