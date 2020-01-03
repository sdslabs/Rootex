#include "timer.h"

Timer::Timer()
    : m_StartTime(s_Clock.now())
{
}

LoggingScopeTimer::LoggingScopeTimer(const String& msg)
    : Timer()
    , m_Message(msg)
{
}

LoggingScopeTimer::~LoggingScopeTimer()
{
	OS::PrintLine(m_Message + " took " + std::to_string(getTimeMs()) + "ms");
}

FrameTimer::FrameTimer()
    : LoggingScopeTimer("FrameTimer")
    , m_FrameCount(0)
    , m_FrameStartTime(s_Clock.now())
{
}

void FrameTimer::reset()
{
	m_FrameStartTime = s_Clock.now();
}

void FrameTimer::showTime()
{
	OS::PrintLine(
	    "Frame " + std::to_string(++m_FrameCount) + 
		" took " + 
		std::to_string((s_Clock.now() - m_FrameStartTime).count() * NS_TO_MS) + "ms");
}
