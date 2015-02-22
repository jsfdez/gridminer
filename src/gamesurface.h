#pragma once

#include "abstractsurface.h"

#include <set>
#include <map>
#include <vector>
#include <memory>

#include "gemsurface.h"

struct Position;
struct SDL_MouseMotionEvent;
struct SDL_MouseButtonEvent;

class GameSurface : public AbstractSurface
{
	enum
	{
		ROWS = 8,
		COLUMNS = ROWS,
		GEM_COUNT = ROWS * COLUMNS,
		OFFSET_X = 330u + GemSurface::WIDTH / 2,
		OFFSET_Y = 100u + GemSurface::HEIGHT / 2,
	};

	enum class Animation
	{
		NO_ANIMATION,
		FALL_ANIMATION,
		SWAPPING_ANIMATION,
		ROLLBACK_ANIMATION,
		DESTROY_ANIMATION,
	} m_animation = Animation::NO_ANIMATION;

	std::shared_ptr<SDL_Surface> m_background;
	std::vector<GemSurface> m_gems;
	std::uint8_t m_selectedGem;
	std::pair<std::uint8_t, std::uint8_t> m_swapping;
	std::set<std::vector<std::uint8_t>> m_destroyingGems;

	enum class Direction
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
	};
	std::set<std::vector<std::uint8_t>> FindGroups() const;
	bool AreContiguous(std::uint8_t first, std::uint8_t second) const;
	void Swap(std::uint8_t first, std::uint8_t second, bool rollback);
	bool FillRow();
	void StartDestruction();

	void OnMouseMoveEvent(const SDL_Event& event);
	void OnMouseClickEvent(const SDL_Event& event);
	void OnMouseReleaseEvent(const SDL_Event& event);

public:
    GameSurface();
    ~GameSurface();

	virtual Status Update(const SDL_Event& event) override;

	virtual Status Update(const std::chrono::time_point<std::chrono::system_clock>& time) override;

	virtual void Render(SDL_Surface& surface) override;
	virtual bool Contains(const Position& position) const override;

	std::uint8_t GetGemIndex(const GemSurface& gem) const;
	Position CalculateGemPosition(std::uint8_t i) const;

	void Create();
	void Destroy();
};
