#pragma once
#include <array>
#include <string_view>

struct GameInfo;
struct PlayerList;
struct HudInfo;
struct MouseInfo;

namespace context {
	inline GameInfo* game_info = nullptr;
	inline PlayerList* player_list = nullptr;
	inline HudInfo* hud_info = nullptr;
	inline MouseInfo* mouse_info = nullptr;
}
