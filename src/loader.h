#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <map>
#include <string>
#include <memory>

#include "gemcolor.h"

struct SDL_Surface;

namespace loader
{
    std::shared_ptr<SDL_Surface> Image(const std::string& file);
	std::shared_ptr<SDL_Surface> GemSurface(GemColor color);
};

#endif // IMAGELOADER_H
