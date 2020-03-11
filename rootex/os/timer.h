#pragma once

#include <chrono>

#include "common/common.h"

#define NS_TO_MS 1e-6f
#define MS_TO_NS 1e+6f
#define MS_TO_S 1e-3f
#define S_TO_MS 1e+3f

class Timer
{
protected:
	static const std::chrono::high_resolution_clock s_Clock;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime;

public:
	Timer();
	Timer(Timer&) = delete;
	virtual ~Timer() = default;

	float getTimeMs() { return (float)(s_Clock.now() - m_StartTime).count() * NS_TO_MS; }
	float getTimeNs() { return (s_Clock.now() - m_StartTime).count(); }
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
};
