#pragma once

#include <glm/vec4.hpp>

#include "gmath/utils.h"

namespace gmath
{
    // Represents a position in a space.
    template<class Space>
    struct position
    {
        using space = Space;
        using glm_vector_type = glm::vec4;

        constexpr explicit position(gmath::no_init_t) noexcept : m_position() {}
        constexpr explicit position(float x = 0.f, float y = 0.f, float z = 0.f, float w = 1.f) noexcept : m_position(x, y, z, w) {}
        constexpr explicit position(glm::vec4 const& position) noexcept: m_position(position) {};
        constexpr position(position const&) noexcept = default;
        constexpr position(position&&) noexcept = default;
        ~position() noexcept = default;

        constexpr position& operator=(position const&) noexcept = default;
        constexpr position& operator=(position&&) noexcept = default;
        constexpr position& operator=(glm_vector_type const& value) { m_position = value; return *this; }
        constexpr position& operator=(glm_vector_type&& value) { m_position = std::move(value); return *this; }

        friend constexpr bool operator==(position const& lhs, position const& rhs) noexcept { return lhs.m_position == rhs.m_position; }
        friend constexpr bool operator!=(position const& lhs, position const& rhs) noexcept { return !(lhs == rhs); }

        constexpr float x() const noexcept { return m_position[0]; }
        constexpr float y() const noexcept { return m_position[1]; }
        constexpr float z() const noexcept { return m_position[2]; }
        constexpr float w() const noexcept { return m_position[3]; }

        constexpr float& x() noexcept { return m_position[0]; }
        constexpr float& y() noexcept { return m_position[1]; }
        constexpr float& z() noexcept { return m_position[2]; }
        constexpr float& w() noexcept { return m_position[3]; }

        constexpr explicit operator glm::vec4() const noexcept { return m_position; }
        constexpr float operator[](std::size_t index) const noexcept { return m_position[static_cast<glm_vector_type::length_type>(index)]; }
        constexpr float& operator[](std::size_t index) noexcept { return m_position[static_cast<glm_vector_type::length_type>(index)]; }
        glm::vec4 get_internal_position() { return m_position; }

    private:
        glm::vec4 m_position;
    };

    template<class Space>
    bool almost_equal(position<Space> const& lhs, position<Space> const& rhs, int ulp = 1) noexcept
    {
        return almost_equal(lhs.x(), rhs.x(), ulp) && almost_equal(lhs.y(), lhs.y(), ulp) && almost_equal(lhs.z(), rhs.z(), ulp);
    }

    template<class Space>
    bool within_epsilon(position<Space> const& lhs, position<Space> const& rhs, float epsilon = 1e-6f) noexcept
    {
        return within_epsilon(lhs.x(), rhs.x(), epsilon) && within_epsilon(lhs.y(), rhs.y(), epsilon) && within_epsilon(lhs.z(), rhs.z(), epsilon);
    }

    template<class Space> position(gmath::position<Space> const&)->position<Space>;
    template<class Space> position(gmath::position<Space>&&)->position<Space>; 
}