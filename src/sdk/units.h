#pragma once
#include "math/vector.h"
#include "math/rotation_matrix.h"
#include "math/view_matrix.h"

struct UnitWeapons {

};

struct UnitInfo {
	char* m_type_name;
	char* m_model_path;
	char* m_blk_path;
	char* m_full_name;
	char* m_short_name;
	char* m_vehicle_type_short;
	char* m_vehicle_type;
	char* m_nation_name;
};

struct Player;
struct Unit {
	auto& bounding_box_min() const noexcept {
		return *reinterpret_cast<Vector*>(std::uintptr_t(this) + 0x200);
	}

	auto& bounding_box_max() const noexcept {
		return *reinterpret_cast<Vector*>(std::uintptr_t(this) + 0x20C);
	}

	auto vehicle_name() const noexcept {
		return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x458);
	}

	auto& rotation_matrix() const noexcept {
		return *reinterpret_cast<RotationMatrix*>(std::uintptr_t(this) + 0x9BC);
	}

	auto& position() const noexcept {
		return *reinterpret_cast<Vector*>(std::uintptr_t(this) + 0x9E0);
	}

	auto& invulnerable() const noexcept {
		return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0xFA0);
	}

	enum class State : std::uint32_t {
		alive = 0,
		dead = 2
	};

	auto& state() const noexcept {
		return *reinterpret_cast<State*>(std::uintptr_t(this) + 0x1080);
	}

	enum class Types {
		aircraft = 0,
		tank = 3,
		boat = 5
	};

	auto& type() const noexcept {
		return *reinterpret_cast<Types*>(std::uintptr_t(this) + 0x1088);
	}

	auto player() const noexcept {
		return *reinterpret_cast<Player**>(std::uintptr_t(this) + 0x1090);
	}

	auto& army_number() const noexcept {
		return *reinterpret_cast<int*>(std::uintptr_t(this) + 0x10D8);
	}

	auto info() const noexcept {
		return *reinterpret_cast<UnitInfo**>(std::uintptr_t(this) + 0x10E8);
	}

	auto weapons() const noexcept {
		return *reinterpret_cast<UnitWeapons**>(std::uintptr_t(this) + 0x1198);
	}
};

struct UnitList {
	Unit** m_units;
	int m_count;
};
