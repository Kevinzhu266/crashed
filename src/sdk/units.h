#pragma once
#include "math/rotation_matrix.h"
#include "math/vector.h"
#include "math/view_matrix.h"

struct UnitWeaponInfo {
    auto name() const noexcept {
        return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x8);
    }

    auto& weapon_info_size() const noexcept {
        return *reinterpret_cast<int*>(std::uintptr_t(this) + 0xB0);
    }
};

struct UnitWeapon {
    auto weapon_info() const noexcept {
        return *reinterpret_cast<UnitWeaponInfo**>(std::uintptr_t(this) + 0x2A8);
    }

    auto& current_ammo_slot() const noexcept {
        return *reinterpret_cast<char*>(std::uintptr_t(this) + 0x524);
    }

    auto& next_ammo_slot() const noexcept {
        return *reinterpret_cast<char*>(std::uintptr_t(this) + 0x525);
    }
};

struct UnitWeaponList {
    UnitWeapon** m_weapons;
    int m_count;
};

struct UnitWeapons {
    auto weapon_list() const noexcept {
        return *reinterpret_cast<UnitWeaponList**>(std::uintptr_t(this) + 0x390);
    }
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

// Commented functions need their offsets updated.
struct Player;
struct Unit {
    auto& bounding_box_min() const noexcept {
        return *reinterpret_cast<Vector*>(std::uintptr_t(this) + 0x2F0);
    }

    auto& bounding_box_max() const noexcept {
        return *reinterpret_cast<Vector*>(std::uintptr_t(this) + 0x2FC);
    }

    //auto vehicle_name() const noexcept {
    //	return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x460);
    //}

    auto& rotation_matrix() const noexcept {
        return *reinterpret_cast<RotationMatrix*>(std::uintptr_t(this) + 0xB4C);
    }

    auto& position() const noexcept {
        return *reinterpret_cast<Vector*>(std::uintptr_t(this) + 0xB70);
    }

    //enum class State : std::uint32_t {
    //	alive = 0,
    //	not_sure = 1,
    //	dead = 2
    //};

    //auto& state() const noexcept {
    //	return *reinterpret_cast<State*>(std::uintptr_t(this) + 0x10A8);
    //}

    //enum class Types {
    //	aircraft = 0,
    //	tank = 3,
    //	boat = 5
    //};

    //auto& type() const noexcept {
    //	return *reinterpret_cast<Types*>(std::uintptr_t(this) + 0x10B0);
    //}

    //auto player() const noexcept {
    //	return *reinterpret_cast<Player**>(std::uintptr_t(this) + 0x10B8);
    //}

    //auto& army_number() const noexcept {
    //	return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + 0x1100);
    //}

    auto model_info() const noexcept {
        return *reinterpret_cast<UnitInfo**>(std::uintptr_t(this) + 0x1110);
    }

    //auto unit_weapons() const noexcept {
    //	return *reinterpret_cast<UnitWeapons**>(std::uintptr_t(this) + 0x11C0);
    //}
};

struct UnitList {
    Unit** m_units;
    int m_count;
};
