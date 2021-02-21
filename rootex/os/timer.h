#pragma once

#include <chrono>

#include "common/common.h"

/// A point in time of the high resolution clock
typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

/// Helper to keep track of time.
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

/// A timer that works like a stopwatch.
class StopTimer : public Timer
{
public:
	StopTimer() = default;
	StopTimer(StopTimer&) = delete;
	virtual ~StopTimer() = default;

	/// Reset start time to now.
	void reset();
};

/// Display a message with the time taken when the scope where this is instantiated ends.
class LoggingScopeTimer : public Timer
{
	String m_Message;

public:
	LoggingScopeTimer(const String& msg);
	LoggingScopeTimer(LoggingScopeTimer&) = delete;
	virtual ~LoggingScopeTimer();
};

/// Timer that helps keep track of frame time.
class FrameTimer : public LoggingScopeTimer
{
	unsigned long long int m_FrameCount;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_FrameStartTime;
	float m_LastFrameTime;

public:
	FrameTimer();
	~FrameTimer() = default;

	/// Reset frame time to 0. Call at the beginning of the frame for accurate results.
	void reset();

	void showTime();
	void showFPS();

	/// Call at the end of the frame for accurate results.
	float getFrameTime() const { return (s_Clock.now() - m_FrameStartTime).count() * NS_TO_MS; }
	float getLastFrameTime() const { return m_LastFrameTime; }
	float getLastFPS() const { return 1.0f / (m_LastFrameTime * MS_TO_S); }
};
