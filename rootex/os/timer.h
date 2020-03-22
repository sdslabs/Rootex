#pragma once

#include <chrono>

#include "common/common.h"

#define NS_TO_MS 1e-6f
#define MS_TO_NS 1e+6f
#define MS_TO_S 1e-3f
#define S_TO_MS 1e+3f

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

class Timer
{
protected:
	static const std::chrono::high_resolution_clock s_Clock;

	TimePoint m_StartTime;
	TimePoint m_EndTime;

public:
	static TimePoint Now();
	
	Timer();
	Timer(Timer&) = delete;
	virtual ~Timer() = default;

	float getTimeMs() const { return (float)(s_Clock.now() - m_StartTime).count() * NS_TO_MS; }
	float getTimeNs() const { return (s_Clock.now() - m_StartTime).count(); }
};

class StopTimer : public Timer
{
public:
	StopTimer() = default;
	StopTimer(StopTimer&) = delete;
	virtual ~StopTimer() = default;

	void reset();
};

class LoggingScopeTimer : public Timer
{
	String m_Message;

public:
	LoggingScopeTimer(const String& msg);
	LoggingScopeTimer(LoggingScopeTimer&) = delete;
	virtual ~LoggingScopeTimer();
};

class FrameTimer : public LoggingScopeTimer
{
	unsigned long long int m_FrameCount;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_FrameStartTime;

public:
	FrameTimer();
	~FrameTimer() = default;

	void reset();
	void showTime();
	void showFPS();

	float getFrameTime() const { return (s_Clock.now() - m_FrameStartTime).count() * NS_TO_MS; }
};
