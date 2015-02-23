#include "abstracttimersurface.h"

#include "constants.h"

void AbstractTimerSurface::Start()
{
	m_started = false;
	m_running = true;
}

void AbstractTimerSurface::Stop()
{
	m_running = m_started = false;
}

AbstractTimerSurface::Status AbstractTimerSurface::Update(
	const std::chrono::time_point<std::chrono::system_clock>& time)
{
	auto ret = Status::CONTINUE;
	if (m_running)
	{
		if (!m_started)
		{
			m_startTime = time;
			m_started = true;
		}
		else
		{
			auto diff = time - m_startTime;
			auto s = std::chrono::duration_cast<std::chrono::seconds>(diff);
			if (s.count() >= constants::MAX_TIME)
			{
				Stop();
				ret = Status::EXIT;
			}
		}
	}
	return ret;
}
