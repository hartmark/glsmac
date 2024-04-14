#pragma once

#include <cstddef>

#include "UnitDef.h"
#include "Slot.h"
#include "Sprite.h"

#include "util/Timer.h"

namespace task {
namespace game {

class Tile;

class Unit {
public:
	// TODO: refactor
	Unit(
		BadgeDefs* badge_defs,
		const size_t id,
		UnitDef* def,
		Slot* slot,
		Tile* tile,
		const Vec3& render_coords,
		const bool is_owned,
		const ::game::unit::Unit::movement_t movement,
		const ::game::unit::Morale::morale_t morale,
		const std::string& morale_string,
		const ::game::unit::Unit::health_t health
	);
	~Unit();

	const size_t GetId() const;
	const bool IsOwned() const;
	const bool IsActive() const;
	Tile* GetTile() const;

	const size_t GetSelectionWeight() const;

	Sprite* GetSprite() const;
	Sprite* GetBadgeSprite() const;
	Sprite* GetBadgeHealthbarSprite() const;

	const std::string GetNameString() const;
	const std::string GetStatsString() const;
	const std::string& GetMoraleString() const;
	const std::string GetMovesString() const;

	void Iterate();

	void SetActiveOnTile();

	void Show();
	void Hide();

	const bool IsBadgeVisible() const;
	void ShowBadge();
	void HideBadge();

	void ShowFakeBadge( const uint8_t offset );
	void HideFakeBadge();

	void StartBadgeBlink();
	void StopBadgeBlink( const bool is_badge_shown );

	void Refresh();

	void SetMovement( const ::game::unit::Unit::movement_t movement );
	void SetHealth( const ::game::unit::Unit::health_t health );
	const bool CanMove() const;

	void MoveTo( Tile* dst_tile, const Vec3& dst_render_coords );

	struct meshtex_t {
		const types::mesh::Mesh* mesh = nullptr;
		types::Texture* texture = nullptr;
	};
	struct render_data_t {
		meshtex_t unit;
		meshtex_t badge;
		meshtex_t healthbar;
		struct {
			std::string name;
			std::string power;
			std::string morale;
			std::string moves;
		} labels;
	};
	const render_data_t& GetRenderData() const;

private:

	BadgeDefs* const m_badge_defs;

	size_t m_id = 0;
	UnitDef* m_def = nullptr;
	Slot* m_slot = nullptr;
	Tile* m_tile = nullptr;
	struct {
		Vec3 coords = {};
		bool is_rendered = false;
		size_t instance_id = 0;
		struct {
			Sprite* def = nullptr;
			size_t instance_id = 0;
			struct {
				Sprite* def = nullptr;
				size_t instance_id = 0;
			} healthbar;
			struct {
				util::Timer timer;
			} blink;
		} badge;
		struct {
			size_t instance_id = 0;
		} fake_badge;
	} m_render;

	const bool m_is_owned = false;
	bool m_is_active = false;
	::game::unit::Unit::movement_t m_movement = 0.0f;
	::game::unit::Morale::morale_t m_morale = 0;
	std::string m_morale_string = "";
	::game::unit::Unit::health_t m_health = 0;

	bool m_need_refresh = true;

	const bool ShouldBeActive() const;

	render_data_t m_render_data = {};
};

}
}

#include "Tile.h"
