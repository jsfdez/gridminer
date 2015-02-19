#ifndef SCENE_H
#define SCENE_H

union SDL_Event;
struct Position;
struct SDL_Surface;

struct AbstractSurface
{
	enum class Status
	{
		CONTINUE,
		EXIT,
	};

	virtual Status Update(const SDL_Event& event) = 0;
	virtual void Render(SDL_Surface& surface) = 0;
	virtual bool Contains(const Position& position) const = 0;
};

#endif // SCENE_H
