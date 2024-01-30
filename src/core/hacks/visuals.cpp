#include "hacks.h"

#include <format>

#include "../../sdk/hud.h"
#include "../../sdk/math/point.h"
#include "../../sdk/math/vector.h"
#include "../../sdk/math/view_matrix.h"
#include "../../sdk/players.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

static bool world_to_screen(const Vector& world, Point& screen) noexcept {
    //const ViewMatrix& vm = context::game_info->view_matrix();
    const auto vm = context::view_matrix->m_data;
    float w = vm[0][3] * world.m_x + vm[1][3] * world.m_y + vm[2][3] * world.m_z + vm[3][3];

    if (w < 0.001f) {
        return false;
    }

    float x = world.m_x * vm[0][0] + world.m_y * vm[1][0] + world.m_z * vm[2][0] + vm[3][0];
    float y = world.m_x * vm[0][1] + world.m_y * vm[1][1] + world.m_z * vm[2][1] + vm[3][1];

    w = 1.f / w;
    float nx = x * w;
    float ny = y * w;

    const ImVec2 size = ImGui::GetIO().DisplaySize;

    screen.m_x = (size.x * 0.5f * nx) + (nx + size.x * 0.5f);
    screen.m_y = -(size.y * 0.5f * ny) + (ny + size.y * 0.5f);

    return true;
}

// TODO: Not use this bs.
#define ARR_TO_IMCOLOR(arr) \
ImColor(arr[0], arr[1], arr[2], arr[3])

void hacks::visuals() noexcept {
    if (!context::game_info || !context::player_list || !context::view_matrix) {
        return;
    }

    const Player* local_player = context::player_list->local_player();

    if (!local_player) {
        return;
    }

    const Unit* local_unit = local_player->owned_unit();

    if (!local_unit) {
        return;
    }

    const UnitList* unit_list = context::game_info->unit_list();

    for (int i = 0; i < unit_list->m_count; ++i) {
        const Unit* unit = unit_list->m_units[i];

        // Make sure unit is alive and not null.
        if (!unit) {
            continue;
        }

        if (hacks::config::esp_enemy_only) {
            if (unit->army_number() == local_unit->army_number()) {
                continue;
            }
        }
        else {
            if (unit == local_unit) {
                continue;
            }
        }

        // Filter out enemies based on runtime config.
        if (hacks::config::esp_enemy_only && unit->army_number() == local_unit->army_number()) {
            continue;
        }

        // Sometimes the game will render inactive players at pos (0, 0, 0).
        Vector& position = unit->position();
        if (position.is_zero()) {
            continue;
        }

        ImDrawList* draw = ImGui::GetBackgroundDrawList();

        // Render 3D boxes. (pasted)
        {
            const RotationMatrix& rotation = unit->rotation_matrix();
            Vector& bb_min = unit->bounding_box_min();
            Vector& bb_max = unit->bounding_box_max();

            Vector ax[6];
            ax[0] = Vector{rotation[0][0], rotation[0][1], rotation[0][2]}.scale(bb_min.m_x);
            ax[1] = Vector{rotation[1][0], rotation[1][1], rotation[1][2]}.scale(bb_min.m_y);
            ax[2] = Vector{rotation[2][0], rotation[2][1], rotation[2][2]}.scale(bb_min.m_z);
            ax[3] = Vector{rotation[0][0], rotation[0][1], rotation[0][2]}.scale(bb_max.m_x);
            ax[4] = Vector{rotation[1][0], rotation[1][1], rotation[1][2]}.scale(bb_max.m_y);
            ax[5] = Vector{rotation[2][0], rotation[2][1], rotation[2][2]}.scale(bb_max.m_z);

            Vector temp[6];
            temp[0] = position + ax[2];
            temp[1] = position + ax[5];
            temp[2] = temp[0] + ax[3];
            temp[3] = temp[1] + ax[3];
            temp[4] = temp[0] + ax[0];
            temp[5] = temp[1] + ax[0];

            Vector v[8];
            v[0] = temp[2] + ax[1];
            v[1] = temp[2] + ax[4];
            v[2] = temp[3] + ax[4];
            v[3] = temp[3] + ax[1];
            v[4] = temp[4] + ax[1];
            v[5] = temp[4] + ax[4];
            v[6] = temp[5] + ax[4];
            v[7] = temp[5] + ax[1];

            Point p1;
            Point p2;
            for (int j = 0; j < 4; ++j) {
                if (world_to_screen(v[j], p1) == true && world_to_screen(v[(j + 1) & 3], p2) == true) {
                    draw->AddLine({p1.m_x, p1.m_y}, {p2.m_x, p2.m_y}, ARR_TO_IMCOLOR(hacks::config::esp_front_colour), 1.f);
                }

                if (world_to_screen(v[4 + j], p1) && world_to_screen(v[4 + ((j + 1) & 3)], p2)) {
                    draw->AddLine({p1.m_x, p1.m_y}, {p2.m_x, p2.m_y}, ARR_TO_IMCOLOR(hacks::config::esp_main_colour), 1.f);
                }

                if (world_to_screen(v[j], p1) && world_to_screen(v[4 + j], p2)) {
                    draw->AddLine({p1.m_x, p1.m_y}, {p2.m_x, p2.m_y}, ARR_TO_IMCOLOR(hacks::config::esp_main_colour), 1.f);
                }
            }
        }
    }
}

//ImColor GetColorForType(const char* type) noexcept {
//	constexpr const auto fighter = fnv::HashConst("exp_fighter");
//	constexpr const auto bomber = fnv::HashConst("exp_bomber");
//	constexpr const auto assault = fnv::HashConst("exp_assault");
//
//	switch (fnv::Hash(type))
//	{
//	case fighter:
//		return ImColor{ 255, 0, 0, 255 };
//	case bomber:
//		return ImColor{ 0, 0, 255, 255 };
//	case assault:
//		return ImColor{ 0, 255, 0, 255 };
//	default:
//		return ImColor{ 200, 200, 200, 155 };
//	}
//}

//void hacks::Visuals() noexcept
//{
//	if (!g::local || !g::local->player || !g::game)
//		return;
//
//	const auto list = g::game->GetUnitList();
//
//	if (!list.unitList)
//		return;
//
//	for (auto i = 0; i < list.unitCount; ++i)
//	{
//		const auto unit = list.unitList->units[i];
//
//		std::cout << std::format("-> unit {:#x}\n", std::uintptr_t(unit));
//
//		if (!unit)
//			continue;
//
//		if (!unit->IsAlive())
//			continue;
//
//		const auto local = g::local->player->GetOwnedUnit();
//
//		if (!local)
//			continue;
//
//		if (unit->GetUnitArmyNumber() == local->GetUnitArmyNumber())
//			continue;
//
//		const auto player = unit->GetPlayer();
//
//		if (player)
//		{
//			const auto& position = unit->GetPosition();
//			const auto distance = position.LengthTo(g::local->player->GetOwnedUnit()->GetPosition()) * 0.001f;
//			const auto& rotation = unit->GetRotationMatrix();
//			const auto& bbmin = unit->GetBBMin();
//			const auto& bbmax = unit->GetBBMax();
//
//			Vector3 ax[6];
//			ax[0] = Vector3{ rotation[0][0], rotation[0][1], rotation[0][2] }.Scale(bbmin.x);
//			ax[1] = Vector3{ rotation[1][0], rotation[1][1], rotation[1][2] }.Scale(bbmin.y);
//			ax[2] = Vector3{ rotation[2][0], rotation[2][1], rotation[2][2] }.Scale(bbmin.z);
//			ax[3] = Vector3{ rotation[0][0], rotation[0][1], rotation[0][2] }.Scale(bbmax.x);
//			ax[4] = Vector3{ rotation[1][0], rotation[1][1], rotation[1][2] }.Scale(bbmax.y);
//			ax[5] = Vector3{ rotation[2][0], rotation[2][1], rotation[2][2] }.Scale(bbmax.z);
//
//			Vector3 temp[6];
//			temp[0] = position + ax[2];
//			temp[1] = position + ax[5];
//			temp[2] = temp[0] + ax[3];
//			temp[3] = temp[1] + ax[3];
//			temp[4] = temp[0] + ax[0];
//			temp[5] = temp[1] + ax[0];
//
//			Vector3 v[8];
//			v[0] = temp[2] + ax[1];
//			v[1] = temp[2] + ax[4];
//			v[2] = temp[3] + ax[4];
//			v[3] = temp[3] + ax[1];
//			v[4] = temp[4] + ax[1];
//			v[5] = temp[4] + ax[4];
//			v[6] = temp[5] + ax[4];
//			v[7] = temp[5] + ax[1];
//
//			const auto draw = ImGui::GetBackgroundDrawList();
//
//			Vector3 p1, p2;
//			for (int i = 0; i < 4; i++)
//			{
//				const auto vehicleType = unit->GetUnitInfo()->vehicleType;
//
//				if (WorldToScreen(v[i], p1) && WorldToScreen(v[(i + 1) & 3], p2))
//					draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, GetColorForType(vehicleType), 2.f);
//
//				if (WorldToScreen(v[4 + i], p1) && WorldToScreen(v[4 + ((i + 1) & 3)], p2))
//					draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, GetColorForType(vehicleType), 2.f);
//
//				if (WorldToScreen(v[i], p1) && WorldToScreen(v[4 + i], p2))
//					draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, GetColorForType(vehicleType), 2.f);
//			}
//
//			if (unit->IsSpotted())
//				continue;
//
//			Vector3 origin = { };
//			if (WorldToScreen(position, origin))
//			{
//				const auto info = unit->GetUnitInfo();
//
//				if (!info)
//					continue;
//
//				const auto text = std::format("{} | {:.{}f} km",
//					info->shortName,
//					distance, 2);
//
//				const auto size = ImGui::CalcTextSize(text.c_str());
//
//				draw->AddRectFilled({ origin.x - (size.x * 0.5f) - 5, origin.y + 5},
//					{ origin.x + (size.x * 0.5f) + 5, origin.y + 10 + (size.y * 0.5f) + 5 }, ImColor(0, 0, 0, 150));
//				draw->AddText({ origin.x - (size.x * 0.5f), origin.y + (size.y * 0.5f) },
//					ImColor(255, 255, 255),
//					text.c_str());
//			}
//		}
//		else
//		{
//			const auto info = unit->GetUnitInfo();
//
//			if (!info)
//				continue;
//
//			const auto name = fnv::Hash(info->fullName);
//
//			if (name == fnv::HashConst("Bridge") ||
//				name == fnv::HashConst("dummy_immortal"))
//				continue;
//
//			const auto& position = unit->GetPosition();
//			const auto& rotation = unit->GetRotationMatrix();
//			const auto& bbmin = unit->GetBBMin();
//			const auto& bbmax = unit->GetBBMax();
//
//			Vector3 ax[6];
//			ax[0] = Vector3{ rotation[0][0], rotation[0][1], rotation[0][2] }.Scale(bbmin.x);
//			ax[1] = Vector3{ rotation[1][0], rotation[1][1], rotation[1][2] }.Scale(bbmin.y);
//			ax[2] = Vector3{ rotation[2][0], rotation[2][1], rotation[2][2] }.Scale(bbmin.z);
//			ax[3] = Vector3{ rotation[0][0], rotation[0][1], rotation[0][2] }.Scale(bbmax.x);
//			ax[4] = Vector3{ rotation[1][0], rotation[1][1], rotation[1][2] }.Scale(bbmax.y);
//			ax[5] = Vector3{ rotation[2][0], rotation[2][1], rotation[2][2] }.Scale(bbmax.z);
//
//			Vector3 temp[6];
//			temp[0] = position + ax[2];
//			temp[1] = position + ax[5];
//			temp[2] = temp[0] + ax[3];
//			temp[3] = temp[1] + ax[3];
//			temp[4] = temp[0] + ax[0];
//			temp[5] = temp[1] + ax[0];
//
//			Vector3 v[8];
//			v[0] = temp[2] + ax[1];
//			v[1] = temp[2] + ax[4];
//			v[2] = temp[3] + ax[4];
//			v[3] = temp[3] + ax[1];
//			v[4] = temp[4] + ax[1];
//			v[5] = temp[4] + ax[4];
//			v[6] = temp[5] + ax[4];
//			v[7] = temp[5] + ax[1];
//
//			const auto draw = ImGui::GetBackgroundDrawList();
//
//			Vector3 p1, p2;
//			for (int i = 0; i < 4; i++)
//			{
//				if (WorldToScreen(v[i], p1) && WorldToScreen(v[(i + 1) & 3], p2))
//					draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, ImColor(255, 255, 0, 155), 2.f);
//
//				if (WorldToScreen(v[4 + i], p1) && WorldToScreen(v[4 + ((i + 1) & 3)], p2))
//					draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, ImColor(255, 255, 0, 155), 2.f);
//
//				if (WorldToScreen(v[i], p1) && WorldToScreen(v[4 + i], p2))
//					draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, ImColor(255, 255, 0, 155), 2.f);
//			}
//		}
//	}
//}

//void hacks::Visuals() noexcept
//{
//	if (!v::player_esp)
//		return;
//
//	if (!g::local || !g::local->player || !g::list)
//		return;
//
//	for (auto i = 0; i < g::list->playerCount; ++i)
//	{
//		const auto entity = g::list->entityList->list[i];
//
//		if (!entity.player)
//			continue;
//
//		if (!entity.player->IsAlive())
//			continue;
//
//		if (entity.player->GetTeam() == g::local->player->GetTeam())
//			continue;
//
//		const auto unit = entity.player->GetOwnedUnit();
//
//		if (!unit)
//			continue;
//
//		const auto& position = unit->GetPosition();
//		const auto distance = position.LengthTo(g::local->player->GetOwnedUnit()->GetPosition()) * 0.001f;
//
//		if (distance > v::player_max_distance)
//			continue;
//
//		const auto& rotation = unit->GetRotationMatrix();
//		const auto& bbmin = unit->GetBBMin();
//		const auto& bbmax = unit->GetBBMax();
//
//		Vector3 ax[6];
//		ax[0] = Vector3{ rotation[0][0], rotation[0][1], rotation[0][2] }.Scale(bbmin.x);
//		ax[1] = Vector3{ rotation[1][0], rotation[1][1], rotation[1][2] }.Scale(bbmin.y);
//		ax[2] = Vector3{ rotation[2][0], rotation[2][1], rotation[2][2] }.Scale(bbmin.z);
//		ax[3] = Vector3{ rotation[0][0], rotation[0][1], rotation[0][2] }.Scale(bbmax.x);
//		ax[4] = Vector3{ rotation[1][0], rotation[1][1], rotation[1][2] }.Scale(bbmax.y);
//		ax[5] = Vector3{ rotation[2][0], rotation[2][1], rotation[2][2] }.Scale(bbmax.z);
//
//		Vector3 temp[6];
//		temp[0] = position + ax[2];
//		temp[1] = position + ax[5];
//		temp[2] = temp[0] + ax[3];
//		temp[3] = temp[1] + ax[3];
//		temp[4] = temp[0] + ax[0];
//		temp[5] = temp[1] + ax[0];
//
//		Vector3 v[8];
//		v[0] = temp[2] + ax[1];
//		v[1] = temp[2] + ax[4];
//		v[2] = temp[3] + ax[4];
//		v[3] = temp[3] + ax[1];
//		v[4] = temp[4] + ax[1];
//		v[5] = temp[4] + ax[4];
//		v[6] = temp[5] + ax[4];
//		v[7] = temp[5] + ax[1];
//
//		const auto draw = ImGui::GetBackgroundDrawList();
//
//		Vector3 p1, p2;
//
//		for (int i = 0; i < 4; i++)
//		{
//			if (WorldToScreen(v[i], p1) && WorldToScreen(v[(i + 1) & 3], p2))
//				draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, GetColorForType(unit->GetUnitInfo()->vehicleType), 2.f);
//
//			if (WorldToScreen(v[4 + i], p1) && WorldToScreen(v[4 + ((i + 1) & 3)], p2))
//				draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, ImColor(255, 255, 255), 2.f);
//
//			if (WorldToScreen(v[i], p1) && WorldToScreen(v[4 + i], p2))
//				draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, ImColor(255, 255, 255), 2.f);
//		}
//
//		//const auto view = position + Vector3{ rotation[0][0], rotation[0][1], rotation[0][2] }.Scale(10.f);
//
//		//Vector3 origin = { };
//		//if (!WorldToScreen(position, origin))
//		//	continue;
//
//		//Vector3 end = { };
//		//if (WorldToScreen(view, end))
//		//	draw->AddLine({ origin.x, origin.y }, { end.x, end.y }, ImColor(255, 0, 0), 2.f);
//
//		Vector3 origin = { };
//		if (!unit->IsSpotted() && WorldToScreen(position, origin))
//		{
//			const auto info = unit->GetUnitInfo();
//
//			if (!info)
//				continue;
//
//			const auto text = std::format("{} | {:.{}f} km",
//				info->shortName,
//				distance, 2);
//
//			const auto size = ImGui::CalcTextSize(text.c_str());
//
//			draw->AddRectFilled({ origin.x + 10, origin.y - (size.y * 0.5f) - 5 },
//				{ origin.x + 20 + size.x, origin.y + (size.y * 0.5f) + 5 }, ImColor(0, 0, 0, 150));
//			draw->AddText({ origin.x + 15, origin.y - (size.y * 0.5f) },
//				ImColor(255, 255, 255),
//				text.c_str());
//		}
//	}
//
//	const auto botList = g::game->GetBotList();
//	const auto list = botList.botEntityList;
//
//	if (!list)
//		return;
//
//	for (auto i = 0; i < botList.maxBots; ++i)
//	{
//		const auto bot = botList.botEntityList->bots[i];
//
//		if (!bot)
//			continue;
//
//		if (!bot->IsAlive())
//			continue;
//
//		const auto unit = g::local->player->GetOwnedUnit();
//
//		if (!unit)
//			continue;
//
//		if (bot->GetUnitArmyNumber() == unit->GetUnitArmyNumber())
//			continue;
//
//		const auto& position = bot->GetPosition();
//		const auto& rotation = bot->GetRotationMatrix();
//		const auto& bbmin = bot->GetBBMin();
//		const auto& bbmax = bot->GetBBMax();
//
//		Vector3 ax[6];
//		ax[0] = Vector3{ rotation[0][0], rotation[0][1], rotation[0][2] }.Scale(bbmin.x);
//		ax[1] = Vector3{ rotation[1][0], rotation[1][1], rotation[1][2] }.Scale(bbmin.y);
//		ax[2] = Vector3{ rotation[2][0], rotation[2][1], rotation[2][2] }.Scale(bbmin.z);
//		ax[3] = Vector3{ rotation[0][0], rotation[0][1], rotation[0][2] }.Scale(bbmax.x);
//		ax[4] = Vector3{ rotation[1][0], rotation[1][1], rotation[1][2] }.Scale(bbmax.y);
//		ax[5] = Vector3{ rotation[2][0], rotation[2][1], rotation[2][2] }.Scale(bbmax.z);
//
//		Vector3 temp[6];
//		temp[0] = position + ax[2];
//		temp[1] = position + ax[5];
//		temp[2] = temp[0] + ax[3];
//		temp[3] = temp[1] + ax[3];
//		temp[4] = temp[0] + ax[0];
//		temp[5] = temp[1] + ax[0];
//
//		Vector3 v[8];
//		v[0] = temp[2] + ax[1];
//		v[1] = temp[2] + ax[4];
//		v[2] = temp[3] + ax[4];
//		v[3] = temp[3] + ax[1];
//		v[4] = temp[4] + ax[1];
//		v[5] = temp[4] + ax[4];
//		v[6] = temp[5] + ax[4];
//		v[7] = temp[5] + ax[1];
//
//		const auto draw = ImGui::GetBackgroundDrawList();
//
//		Vector3 p1, p2;
//
//		for (int i = 0; i < 4; i++)
//		{
//			if (WorldToScreen(v[i], p1) && WorldToScreen(v[(i + 1) & 3], p2))
//				draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, ImColor(255, 255, 0), 2.f);
//
//			if (WorldToScreen(v[4 + i], p1) && WorldToScreen(v[4 + ((i + 1) & 3)], p2))
//				draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, ImColor(255, 255, 0), 2.f);
//
//			if (WorldToScreen(v[i], p1) && WorldToScreen(v[4 + i], p2))
//				draw->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, ImColor(255, 255, 0), 2.f);
//		}
//	}
//}
