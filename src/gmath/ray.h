#pragma once

#include <algorithm>

#include "gmath/utils.h"
#include "gmath/vector.h"
#include "gmath/transform.h"

namespace gmath
{
    template<class Space>
    struct ray
    {
        using space = Space;

        constexpr ray(position<space> const& start, vector<space> const& direction) noexcept
            : m_start(start), m_direction(direction) {}
        constexpr ray(position<space> const& start, vector<space> const& direction, float minT, float maxT, int depth) noexcept
            : m_start(start), m_direction(direction), m_minT(minT), m_maxT(maxT), m_depth(depth) {}
        constexpr ray(position<space> const& start, vector<space> const& direction, ray const& parent) noexcept
            : m_start(start), m_direction(direction), m_minT(parent.m_minT), m_maxT(parent.m_maxT), m_depth(parent.m_depth + 1) {}
        constexpr ray() noexcept = default;
        constexpr ray(ray const&) noexcept = default;
        constexpr ray(ray&&) noexcept = default;
        ~ray() noexcept = default;

        constexpr ray& operator=(ray const&) noexcept = default;
        constexpr ray& operator=(ray&&) noexcept = default;

        constexpr position<space> const& start() const noexcept { return m_start; }
        constexpr vector<space> const& dir() const noexcept { return m_direction; }
        constexpr float minT() const noexcept { return m_minT; }
        constexpr float maxT() const noexcept { return m_maxT; }
        constexpr int depth() const noexcept { return m_depth; }
        
        position<space>& start() noexcept { return m_start; }
        vector<space>& dir() noexcept { return m_direction; }
        void set_minT(float minT) noexcept { m_minT = minT; if (m_minT > m_maxT) std::swap(m_minT, m_maxT); }
        void set_maxT(float maxT) noexcept { m_maxT = minT; if (m_maxT < m_minT) std::swap(m_minT, m_maxT); }
        
        bool is_valid_t(float t) const noexcept { return t >= m_minT && t <= m_maxT; }

        constexpr position<space> operator()(float time) const noexcept { return m_start + m_direction * time; }

        template<class ToSpace>
        friend ray<ToSpace> operator*(transform<ToSpace,Space> const& lhs, ray const& rhs) noexcept
        {
            return ray<ToSpace>(lhs * rhs.m_start, lhs * rhs.m_direction, rhs.m_minT, rhs.m_maxT, rhs.m_depth);
        }

    private:
        position<space> m_start;
        vector<space> m_direction;
        float m_minT = std::numeric_limits<float>::epsilon();
        float m_maxT = std::numeric_limits<float>::max();
        int m_depth = 0;
    };
}