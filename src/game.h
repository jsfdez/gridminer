#ifndef GAME_H
#define GAME_H

#include "abstractscene.h"

#include <map>
#include <array>
#include <memory>

class Game : public AbstractScene
{
	enum class Gem
	{
		BLUE,
		GREEN,
		PURPLE,
		RED,
		YELLOW,
	};

	std::shared_ptr<SDL_Surface> m_background;
	std::array<Gem, 8 * 8> m_gridData;
	std::map<Gem, std::shared_ptr<SDL_Surface>> m_gems;

public:
    Game();
    ~Game();

	virtual Status Update(const SDL_Event& event, SDL_Surface& surface) override;

	void Create();
	void Destroy();
};

#endif // GAME_H
