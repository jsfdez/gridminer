#ifndef SCENE_H
#define SCENE_H

#include "SDL_events.h"

struct AbstractScene
{
	enum class Status
	{
		CONTINUE,
		EXIT,
	};

	virtual Status Update(const SDL_Event& event, SDL_Surface& surface) = 0;
};

#endif // SCENE_H