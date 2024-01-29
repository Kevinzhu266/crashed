#pragma once
#include "../../sdk/game.h"
#include "../context.h"
#include "../memory.h"

namespace hacks {
    namespace config {
        inline bool watermark = true;

        inline bool esp_enabled = true;
        inline bool esp_enemy_only = false;
        inline std::array<float, 4> esp_main_colour = {1.f, 1.f, 1.f, 0.6f};
        inline std::array<float, 4> esp_front_colour = {1.f, 0.f, 0.f, 0.6f};
    }

    void visuals() noexcept;
}
