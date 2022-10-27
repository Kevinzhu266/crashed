#pragma once
#include <array>
#include <string_view>

struct GameInfo;
struct PlayerList;

namespace context {
	inline GameInfo* game_info = nullptr;
	inline PlayerList* player_list = nullptr;
}
