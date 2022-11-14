#pragma once
#include <cstdint>

struct HudInfo {
    auto& penetration_crosshair() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x28C);
    }

    auto& unit_glow() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x28D);
    }

    auto& gunner_sight_distance() const noexcept {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + 0x28E);
    }
};
