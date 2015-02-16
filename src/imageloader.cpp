#include "imageloader.h"

#include "SDL.h"
#include "SDL_image.h"

std::map<std::string, std::weak_ptr<SDL_Surface>> ImageLoader::s_data;

std::shared_ptr<SDL_Surface> ImageLoader::Load(const std::string& file)
{
    auto it = s_data.find(file);
    if (it != s_data.end() && !it->second.expired())
        return it->second.lock();
    else
    {
        auto ptr = std::shared_ptr<SDL_Surface>(IMG_Load(file.c_str()),
            SDL_FreeSurface);
        if (ptr)
        {
            s_data[file] = ptr;
        }
        return ptr;
    }
}
