#pragma once
#include "syscall.h"

namespace memory {
	std::uintptr_t scan(const char* signature) noexcept;

	template<typename T = std::uintptr_t>
	T get_offset(std::uintptr_t addr, int offset) noexcept {
		return reinterpret_cast<T>(addr + static_cast<int>((*reinterpret_cast<int*>(addr + offset) + offset) + sizeof(int)));
	}
}
