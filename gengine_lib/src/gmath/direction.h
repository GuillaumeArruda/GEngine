#pragma once

#include <glm/glm.hpp>

#include "gmath/utils.h"

namespace gmath
{
    // Always normalized vector
    template<class Space>
    struct direction
    {
        using space = Space;
        using glm_vector_type = glm::vec3;
        enum class garantee_normal_t
        {
            garantee_normal
        };

        static constexpr direction forward() noexcept { return direction(garantee_normal_t::garantee_normal, 0.f, 0.f, 1.f); }
        static constexpr direction up() noexcept { return direction(garantee_normal_t::garantee_normal, 0.f, 1.f, 0.f); }
        static constexpr direction right() noexcept { return direction(garantee_normal_t::garantee_normal, 1.f, 0.f, 0.f); }

        constexpr explicit direction(glm_vector_type const& vector) : m_direction(vector) { normalize(); }
        constexpr direction(float x = 1.f, float y = 0.f, float z = 0.f) noexcept : m_direction(x, y, z) { normalize(); }
        constexpr direction(garantee_normal_t, float x, float y, float z) noexcept : m_direction(x,y,z) {}
        constexpr direction(garantee_normal_t, glm_vector_type const& vector) noexcept : m_direction(vector) {}
        constexpr direction(direction&&) noexcept = default;
        constexpr direction(direction const&) noexcept = default;
        ~direction() noexcept = default;

        constexpr direction& operator=(direction const&) = default;
        constexpr direction& operator=(direction&&) = default;

        constexpr explicit operator glm_vector_type() const noexcept { return m_direction; }

        friend constexpr bool operator==(direction const& lhs, direction const& rhs) noexcept { return lhs.m_direction == rhs.m_direction; }
        friend constexpr bool operator!=(direction const& lhs, direction const& rhs) noexcept { return !(lhs == rhs); }

        template<template<typename> class VectorType>
        constexpr float dot(VectorType<Space> const& vector) const noexcept { return glm::dot(m_direction, static_cast<typename VectorType<Space>::glm_vector_type>(vector)); }
        template<template<typename> class VectorType>
        constexpr float distance(VectorType<Space> const& vector) const noexcept { return glm::distance(direction, static_cast<typename VectorType<Space>::glm_vector_type>(vector)); }

        friend constexpr direction operator-(direction const& lhs) { return direction(garantee_normal_t::garantee_normal, -lhs.m_direction); }

        constexpr direction cross(direction const& rhs) const noexcept { return direction(garantee_normal_t::garantee_normal, glm::cross(m_direction, rhs.m_direction)); }
        constexpr direction reflect(direction const& incident) const noexcept { return direction(garantee_normal_t::garantee_normal, glm::reflect(m_direction, incident.m_direction)); }
        constexpr direction refract(direction const& normal, float eta) const noexcept { return glm::refract(m_direction, normal.m_direction, eta); }

        constexpr float x() const noexcept { return m_direction[0]; }
        constexpr float y() const noexcept { return m_direction[1]; }
        constexpr float z() const noexcept { return m_direction[2]; }
    private:
        constexpr void normalize() noexcept { m_direction = glm::normalize(m_direction); }
        glm_vector_type m_direction;
    };

    template<class Space>
    bool almost_equal(direction<Space> const& lhs, direction<Space> const& rhs, int ulp = 1) noexcept
    {
        return almost_equal(lhs.x(), rhs.x(), ulp) && almost_equal(lhs.y(), lhs.y(), ulp) && almost_equal(lhs.z(), rhs.z(), ulp);
    }

    template<class Space>
    bool within_epsilon(direction<Space> const& lhs, direction<Space> const& rhs, float epsilon = 1e-6f) noexcept
    {
        return within_epsilon(lhs.x(), rhs.x(), epsilon) && within_epsilon(lhs.y(), rhs.y(), epsilon) && within_epsilon(lhs.z(), rhs.z(), epsilon);
    }

    template<class Space> direction(gmath::direction<Space> const&)->direction<Space>;
    template<class Space> direction(gmath::direction<Space>&&)->direction<Space>;
}