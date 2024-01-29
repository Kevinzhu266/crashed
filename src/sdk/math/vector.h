#pragma once
#include <cmath>

struct Vector {
    Vector() noexcept
        : m_x(0), m_y(0), m_z(0) {}

    Vector(float x, float y, float z) noexcept
        : m_x(x), m_y(y), m_z(z) {}

    Vector operator+(const Vector& other) noexcept {
        return Vector{m_x + other.m_x, m_y + other.m_y, m_z + other.m_z};
    }

    Vector operator-(const Vector& other) noexcept {
        return Vector{m_x - other.m_x, m_y - other.m_y, m_z - other.m_z};
    }

    bool is_zero() const noexcept {
        return std::fpclassify(m_x) == FP_ZERO
            && std::fpclassify(m_y) == FP_ZERO
            && std::fpclassify(m_z) == FP_ZERO;
    }

    float len_to(const Vector& vec) const noexcept {
        return std::sqrt((vec.m_x - m_x) * (vec.m_x - m_x)
            + (vec.m_y - m_y) * (vec.m_y - m_y)
            + (vec.m_z - m_z) * (vec.m_z - m_z));
    }

    Vector& scale(float factor) noexcept {
        m_x *= factor;
        m_y *= factor;
        m_z *= factor;

        return *this;
    }

    float m_x;
    float m_y;
    float m_z;
};
