#pragma once
#include <array>
#include <string_view>

struct GameInfo;
struct PlayerList;
struct HudInfo;

namespace context {
	inline GameInfo* game_info = nullptr;
	inline PlayerList* player_list = nullptr;
	inline HudInfo* hud_info = nullptr;
}
