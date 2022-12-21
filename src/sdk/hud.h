#pragma once
#include <cstdint>

struct HudInfo {
    auto& allow_third_person() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x44);
    }

    auto& damage_indicator() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x28C);
    }

    auto& unit_glow() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x28D);
    }

    auto& air_to_air_indicator() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x281);
    }

    auto& gunner_sight_distance() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x28E);
    }

    // realistic mouse aim controls
    auto& mouse_aim() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x2D1);
    }

    // simulator controls
    auto& sim_aim() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x2D2);
    }

    auto& bomb_indicator() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x2D4);
    }

    auto& show_marker_arrows() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x283);
    }

    auto& show_enemies_on_mini_map() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x285);
    }

    auto& show_mini_map() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x286);
    }

    auto& show_aircraft_damage_model() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x289);
    }
};
