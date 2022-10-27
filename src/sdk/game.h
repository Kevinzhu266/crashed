#pragma once
#include "units.h"

struct GameInfo {
	auto map() const noexcept {
		return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x1D0);
	}

	auto time() const noexcept {
		return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x1D8);
	}

	auto weather() const noexcept {
		return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x1E0);
	}

	auto mission() const noexcept {
		return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x200);
	}

	auto bot_list_one() const noexcept {
		return *reinterpret_cast<UnitList**>(std::uintptr_t(this) + 0x350);
	}

	auto bot_list_two() const noexcept {
		return *reinterpret_cast<UnitList**>(std::uintptr_t(this) + 0x370);
	}

	auto bot_list_three() const noexcept {
		return *reinterpret_cast<UnitList**>(std::uintptr_t(this) + 0x390);
	}

	auto& view_matrix() const noexcept {
		auto location = *reinterpret_cast<const std::uintptr_t*>(std::uintptr_t(this) + 0x7b0);
		return *reinterpret_cast<ViewMatrix*>(location + 0x268);
	}
};
