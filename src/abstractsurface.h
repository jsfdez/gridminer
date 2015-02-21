#ifndef SCENE_H
#define SCENE_H

#include <chrono>

union SDL_Event;
struct Position;
struct SDL_Surface;

struct AbstractSurface
{
	enum class Status
	{
		CONTINUE,
		ANIMATION,
		EXIT,
	};

	virtual Status Update(const SDL_Event& event) = 0;
	virtual Status Update(const std::chrono::time_point<std::chrono::system_clock>& time) = 0;

	virtual void Render(SDL_Surface& surface) = 0;
	virtual bool Contains(const Position& position) const = 0;
};

#endif // SCENE_H
