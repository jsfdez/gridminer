#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <map>
#include <string>
#include <memory>

struct SDL_Surface;

struct ImageLoader
{
    static std::shared_ptr<SDL_Surface> Load(const std::string& file);

private:
    static std::map<std::string, std::weak_ptr<SDL_Surface>> s_data;
};

#endif // IMAGELOADER_H
