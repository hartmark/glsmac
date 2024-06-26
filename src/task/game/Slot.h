#pragma once

#include <cstddef>
#include <cstdint>

namespace task {
namespace game {

namespace faction {
class Faction;
}

class Slot {
public:

	Slot(
		const size_t slot_index,
		faction::Faction* faction
	);

	const size_t GetIndex() const;
	faction::Faction* GetFaction() const;

private:

	const size_t m_slot_index;
	faction::Faction* m_faction;

};

}
}
