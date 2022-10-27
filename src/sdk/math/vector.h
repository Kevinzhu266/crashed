#pragma once
#include <cmath>

struct Vector {
	Vector() noexcept
		: m_x(0), m_y(0), m_z(0) {}

	Vector(float x, float y, float z) noexcept
		: m_x(x), m_y(y), m_z(z) {}

	Vector& operator+(const Vector& other) noexcept {
		m_x += other.m_x;
		m_y += other.m_y;
		m_z += other.m_z;
		return *this;
	}

	Vector& operator-(const Vector& other) noexcept {
		m_x -= other.m_x;
		m_y -= other.m_y;
		m_z -= other.m_z;
		return *this;
	}

	float distance_between(const Vector& vec) noexcept {
		return std::sqrt((vec.m_x - m_x) * (vec.m_x - m_x) +
			(vec.m_y - m_y) * (vec.m_y - m_y) +
			(vec.m_z - m_z) * (vec.m_z - m_z));
	}

	float m_x;
	float m_y;
	float m_z;
};
