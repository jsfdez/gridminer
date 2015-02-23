#pragma once

#include "abstractsurface.h"

struct AbstractTimerSurface : AbstractSurface
{
	virtual void Start();
	void Stop();

	virtual Status Update(const std::chrono::time_point<std::chrono::system_clock>& time) override;

protected:
	std::chrono::system_clock::time_point m_startTime;
	bool m_started = false;
	bool m_running = false;
};
