#include "timer.h"
#include <Windows.h>

Timer::Timer() : m_SecondsPerCount(0.0), m_DeltaTime(-1.0), m_BaseTime(0), m_PausedTime(0), m_StopTime(0), m_CurrentTime(0), m_PrevTime(0), m_Stopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0 / (double)countsPerSec;
};

float Timer::TotalTimeSpent() const
{
	if (m_Stopped)
	{
		// last time the clock was stopped : m_StopTime (here also the current time in the app, because time is stopped)
		// the time when the clock was first start : m_BaseTime
		// the amount of time the clock stayed paused : m_PausedTime
		return (float)(((m_StopTime - m_BaseTime) - m_PausedTime) * m_SecondsPerCount);
	}
	else {
		// Current time because time isn't stopped : m_CurrentTime
		// the time when the clock was first start : m_BaseTime
		// the amount of time the clock stayed paused : m_PausedTime
		return (float)(((m_CurrentTime - m_BaseTime) - m_PausedTime) * m_SecondsPerCount);
	}
};

float Timer::TimeSinceLastFrame() const
{
	return (float)m_DeltaTime;
};

void Timer::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_BaseTime = currentTime;
	m_PausedTime = currentTime;
	m_Stopped = 0;
	m_Stopped = false;
};

void Timer::Start()
{
	if (m_Stopped) {

		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		m_PausedTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_Stopped = false;
	}
};

void Timer::Stop()
{
	if (!m_Stopped)
	{
		__int64 stopTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&stopTime);

		m_StopTime = stopTime;
		m_Stopped = true;
	}
};

void Timer::Tick()
{
	if (m_Stopped) 
	{
		m_DeltaTime = 0;
		return;
	}

	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	m_CurrentTime = currentTime;

	m_DeltaTime = (m_CurrentTime - m_PrevTime) * m_CurrentTime;

	m_PrevTime = m_CurrentTime;

	if (m_DeltaTime < 0.0)
		m_DeltaTime = 0.0;
};



