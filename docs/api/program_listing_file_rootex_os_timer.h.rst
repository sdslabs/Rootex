
.. _program_listing_file_rootex_os_timer.h:

Program Listing for File timer.h
================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_os_timer.h>` (``rootex\os\timer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <chrono>
   
   #include "common/common.h"
   
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
       float m_LastFrameTime;
   
   public:
       FrameTimer();
       ~FrameTimer() = default;
   
       void reset();
   
       void showTime();
       void showFPS();
   
       float getFrameTime() const { return (s_Clock.now() - m_FrameStartTime).count() * NS_TO_MS; }
       float getLastFrameTime() const { return m_LastFrameTime; }
       float getLastFPS() const { return 1.0f / (m_LastFrameTime * MS_TO_S); }
   };
