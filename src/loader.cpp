#include "loader.h"

#include <vector>

#include "SDL.h"
#include "SDL_image.h"

#include "files.h"
#include "gemsurface.h"

static std::map<std::string, std::weak_ptr<SDL_Surface>> s_images;
static std::map<GemColor, std::weak_ptr<SDL_Surface>> s_gems;
static std::map<GemColor, std::string> s_gemFileNames
{
	{ GemColor::BLUE,	k_imageGemBlue },
	{ GemColor::GREEN,	k_imageGemGreen },
	{ GemColor::PURPLE, k_imageGemPurple },
	{ GemColor::RED,	k_imageGemRed },
	{ GemColor::YELLOW, k_imageGemYellow },
};

std::shared_ptr<SDL_Surface> loader::Image(const std::string& file)
{
    auto it = s_images.find(file);
    if (it != s_images.end() && !it->second.expired())
        return it->second.lock();
    else
    {
        auto ptr = std::shared_ptr<SDL_Surface>(IMG_Load(file.c_str()),
            SDL_FreeSurface);
        if (ptr)
        {
            s_images[file] = ptr;
        }
        return ptr;
    }
}

std::shared_ptr<SDL_Surface> CreateGemAnimations(SDL_Surface& surface)
{
	const auto& f = surface.flags;
	const auto& format = surface.format;
	const auto& width = surface.w;
	const auto& height = surface.h;
	const auto& depth = format->BitsPerPixel;
	const auto& Rmask = format->Rmask;
	const auto& Gmask = format->Gmask;
	const auto& Bmask = format->Bmask;
	const auto& Amask = format->Amask;

	auto ret = std::shared_ptr<SDL_Surface>(SDL_CreateRGBSurface(surface.flags,
		GemSurface::WIDTH, GemSurface::HEIGHT, format->BitsPerPixel,
		format->Rmask, format->Gmask, format->Bmask,
		format->Amask), SDL_FreeSurface
	);
	typedef std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> Frame;
	std::vector<Frame> frames;
	{
		frames.emplace_back(SDL_CreateRGBSurface(f, width, height, depth, Rmask,
			Gmask, Bmask, Amask), SDL_FreeSurface);
		SDL_BlitSurface(&surface, nullptr, frames.back().get(), nullptr);
	}

	static_assert(GemSurface::WIDTH == GemSurface::HEIGHT, "Squares");
	// Fadeout animation: 255 frames
	for (auto alpha = 0xFFu; alpha > 0x00; --alpha)
	{
		const std::size_t pixelCount = width * height;
		frames.emplace_back(SDL_CreateRGBSurface(f, width, height, depth,
			Rmask, Gmask, Bmask, Amask), SDL_FreeSurface);
		auto pixels = reinterpret_cast<std::uint32_t*>(surface.pixels);
		auto destPixels = reinterpret_cast<std::uint32_t*>(frames.back()->pixels);
		for (auto i = 0u; i < pixelCount; ++i)
		{
			if ((pixels[i] & Amask) == Amask)
			{
				destPixels[i] = (pixels[i] & ~Amask)
					+ (alpha << format->Ashift);
			}
			else if ((pixels[i] & Amask) == Amask) destPixels[i] = 0;
			else destPixels[i] = pixels[i];
		}
	}
	// Vertical destruction: 42 frames
	for (auto i = 1; i < width; ++i)
	{
		SDL_Rect rect;
		rect.x = i / 2;
		rect.y = 0;
		rect.w = width - i;
		rect.h = height;
		frames.emplace_back(SDL_CreateRGBSurface(f, width, height, depth, 
			Rmask, Gmask, Bmask, Amask), SDL_FreeSurface);
		auto result = SDL_SoftStretch(&surface, nullptr, frames.back().get(), 
			&rect);
		if (result)
			SDL_Log("%s", SDL_GetError());
		SDL_assert(!result);
	}
	// Horizontal destruction: 42 frames
	for (auto i = 1; i < height; ++i)
	{
		SDL_Rect rect;
		rect.x = 0;
		rect.y = i / 2;
		rect.w = width;
		rect.h = height - i;
		frames.emplace_back(SDL_CreateRGBSurface(f, width, height, depth,
			Rmask, Gmask, Bmask, Amask), SDL_FreeSurface);
		auto result = SDL_SoftStretch(&surface, nullptr, frames.back().get(),
			&rect);
		if (result)
			SDL_Log("%s", SDL_GetError());
		SDL_assert(!result);
	}

	ret = std::shared_ptr<SDL_Surface>(SDL_CreateRGBSurface(f, 
		GemSurface::WIDTH * frames.size(), height, depth, Rmask, Gmask, Bmask, 
		Amask), SDL_FreeSurface);

	for (decltype(frames.size()) i = 0; i < frames.size(); ++i)
	{
		SDL_Rect rect{ i * GemSurface::WIDTH, 0, GemSurface::WIDTH, 
			GemSurface::HEIGHT };
		SDL_BlitSurface(frames[i].get(), nullptr, ret.get(), &rect);
	}

	return ret;
}

std::shared_ptr<SDL_Surface> loader::GemSurface(GemColor color)
{
	decltype(GemSurface(color)) ptr;

	if (GemColor::EMPTY == color)
		return ptr;

	auto it = s_gems.find(color);
	if (it != s_gems.end() && !it->second.expired())
		ptr = it->second.lock();
	else
	{
		int result;
		auto image = Image(s_gemFileNames[color]);
		const auto& format = image->format;
		auto resized = std::shared_ptr<SDL_Surface>(SDL_CreateRGBSurface(
			image->flags, GemSurface::WIDTH, GemSurface::HEIGHT, 
			format->BitsPerPixel, format->Rmask, format->Gmask, format->Bmask,
			format->Amask), SDL_FreeSurface);
		SDL_assert(resized);
//#define STRECH_GEM
#ifdef STRECH_GEM
		SDL_Rect srcRect{ 0, 0, image->w, image->h };
		SDL_Rect destRect { 0, 0, GemSurface::WIDTH, GemSurface::HEIGHT };
		result = SDL_SoftStretch(image.get(), &srcRect, resized.get(), 
			&destRect);
#else
		SDL_Rect destRect{ 
			(GemSurface::WIDTH - image->w) / 2, 
			(GemSurface::HEIGHT - image->h) / 2,
			GemSurface::WIDTH,
			GemSurface::HEIGHT
		};
		result = SDL_BlitSurface(image.get(), nullptr, resized.get(),
			&destRect);
#endif
		if (result)
			SDL_Log("Error creating Gem", SDL_GetError());
		ptr = CreateGemAnimations(*resized);
		//ptr = resized;
		s_gems[color] = ptr;
	}
	return ptr;
}

loader::AnimationData loader::GetAnimationData(Animation animation)
{
	static std::vector<std::pair<Animation, std::uint32_t>> sizes { 
		{ Animation::NO_ANIMATION, 1 },
		{ Animation::FADEOUT, 0xFF },
		{ Animation::DESTRUCTION_VERTICAL, GemSurface::WIDTH },
		{ Animation::DESTRUCTION_HORIZONTAL, GemSurface::HEIGHT },
	};
	AnimationData ret{ 0, 0 };
	for (auto& size : sizes)
	{
		ret.m_offset += ret.m_duration;
		ret.m_duration = size.second;
		if (animation == size.first) return ret;
	}
	return ret;
}
