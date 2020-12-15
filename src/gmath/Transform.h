#pragma once

#include <type_traits>

#include <glm/mat4x4.hpp>

#include "gmath/utils.h"
#include "gmath/position.h"

namespace gmath
{
    template<class ToSpace, class FromSpace>
    struct transform
    {
        using from_space = FromSpace;
        using to_space = ToSpace;

        constexpr transform() noexcept : m_transform(1.f) {}
        constexpr explicit transform(gmath::no_init_t) noexcept : m_transform() {}
        constexpr explicit transform(glm::mat4 const& transform) noexcept : m_transform(transform) {}
        constexpr transform(transform const&) noexcept = default;
        constexpr transform(transform&&) noexcept = default;
        ~transform() noexcept = default;

        constexpr transform& operator=(transform const&) noexcept = default;
        constexpr transform& operator=(transform&&) noexcept = default;
        
        constexpr explicit operator glm::mat4() const noexcept { return m_transform; }

        template<template<typename> class Vector, class glm_vector_type = Vector<from_space>::glm_vector_type> 
        friend constexpr auto operator*(transform const& transform, Vector<from_space> const& rhs) noexcept
        {
            if constexpr (std::is_same_v<glm_vector_type, glm::vec3>)
            {
                return Vector<to_space>(static_cast<glm::mat3>(transform.m_transform) * static_cast<glm_vector_type>(rhs));
            }
            else
            {
                return Vector<to_space>(transform.m_transform * static_cast<glm_vector_type>(rhs));
            }
        }

        template<class LHSToSpace>
        friend auto operator*(transform<LHSToSpace, to_space> const& lhs, transform const& rhs) noexcept
        {
            return transform<LHSToSpace, from_space>(static_cast<glm::mat4>(lhs) * rhs.m_transform);
        }

        position<to_space> get_translation() const noexcept { return position<to_space>(m_transform[3]); }

        constexpr transform<from_space, to_space> inverse() const noexcept { return transform<from_space, to_space>(glm::inverse(m_transform)); }

        friend constexpr bool operator==(transform const& lhs, transform const& rhs) noexcept { return lhs.m_transform == rhs.m_transform; }
        friend constexpr bool operator!=(transform const& lhs, transform const& rhs) noexcept { return !(lhs == rhs); }
        glm::mat4& get_internal_matrix() { return m_transform; }
    private:
        glm::mat4 m_transform;
    };
}