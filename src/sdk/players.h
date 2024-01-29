#pragma once
#include "math/rotation_matrix.h"
#include "math/vector.h"
#include "math/view_matrix.h"

struct Unit;
struct Player {
    //auto full_name() const noexcept {
    //    return *reinterpret_cast<char**>(std::uintptr_t(this) + 0x60);
    //}

    //auto user_id() const noexcept {
    //    return *reinterpret_cast<char**>(std::uintptr_t(this) + 0xB0);
    //}

    //auto clan_tag() const noexcept {
    //    return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x168);
    //}

    //auto title() const noexcept {
    //    return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x190);
    //}

    auto& team() const noexcept {
        return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + 0x208);
    }

    //auto& time_to_respawn() const noexcept {
    //    return *reinterpret_cast<float*>(std::uintptr_t(this) + 0x448);
    //}

    //auto& time_to_kick() const noexcept {
    //    return *reinterpret_cast<float*>(std::uintptr_t(this) + 0x4C0);
    //}

    enum class GuiState : std::uint8_t {
        lobby = 1,
        alive = 2,
        vehicle_select = 6
    };

    auto& gui_state() const noexcept {
        return *reinterpret_cast<GuiState*>(std::uintptr_t(this) + 0x4F8);
    }

    auto owned_unit() const noexcept {
        return *reinterpret_cast<Unit**>(std::uintptr_t(this) + 0x6F8);
    }
};

struct PlayerList {
    auto local_player() const noexcept {
        return *reinterpret_cast<Player**>(std::uintptr_t(this) + 0x18);
    }

    Player** m_players;
private:
    char m_pad_0008[8];
public:
    int m_count;
};
