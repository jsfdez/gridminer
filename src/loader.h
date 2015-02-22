#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <map>
#include <string>
#include <memory>

#include "gemcolor.h"

struct SDL_Surface;

namespace loader
{
	enum class Animation
	{
		NO_ANIMATION,
		FADEOUT,
		DESTRUCTION_VERTICAL,
		DESTRUCTION_HORIZONTAL
	};
	struct AnimationData
	{
		std::uint32_t m_offset;
		std::uint32_t m_duration;
	};

    std::shared_ptr<SDL_Surface> Image(const std::string& file);
	std::shared_ptr<SDL_Surface> GemSurface(GemColor color);
	AnimationData GetAnimationData(Animation animation);
};

#endif // IMAGELOADER_H
