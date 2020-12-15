#pragma once

#include "gmath/vector.h"
#include "gmath/position.h"
#include "gmath/ray.h"

namespace gmath
{
    template<class Space>
    struct axis_aligned_box
    {
        axis_aligned_box(gmath::position<Space> const& min, gmath::position<Space> const& max) 
            : m_min(glm::min(static_cast<glm::vec4>(min), static_cast<glm::vec4>(max)))
            , m_max(glm::max(static_cast<glm::vec4>(min), static_cast<glm::vec4>(max))) {}
        axis_aligned_box(axis_aligned_box const&) noexcept = default;
        axis_aligned_box(axis_aligned_box&&) noexcept = default;
        ~axis_aligned_box() noexcept = default;

        axis_aligned_box& operator=(axis_aligned_box const&) noexcept = default;
        axis_aligned_box& operator=(axis_aligned_box&&) noexcept = default;

        axis_aligned_box merge(axis_aligned_box const& rhs) const noexcept
        {
            return axis_aligned_box(
                gmath::position<Space>(glm::min(static_cast<glm::vec4>(m_min), static_cast<glm::vec4>(rhs.m_min))),
                gmath::position<Space>(glm::max(static_cast<glm::vec4>(m_max), static_cast<glm::vec4>(rhs.m_max))));
        }

        struct intersect_result
        {
            float t0 = 0.f;
            float t1 = 0.f;
        };

        std::optional<intersect_result> intersect(gmath::ray<Space> const& ray) const noexcept
        {
            float t0 = ray.minT();
            float t1 = ray.maxT();
            for (int i = 0; i < 3; ++i)
            {
                float const inv_ray_dir = 1.f / ray.dir()[i];
                float t_near = (m_min[i] - ray.start()[i]) * inv_ray_dir;
                float t_far = (m_max[i] - ray.start()[i]) * inv_ray_dir;
                if (t_near > t_far) std::swap(t_near, t_far);
                t0 = t_near > t0 ? t_near : t0;
                t1 = t_far < t1 ? t_far : t1;
                if (t0 > t1) return {};
            }

            return intersect_result{t0, t1};
        }

        float surface_area() const noexcept
        {
            gmath::vector<Space> const diff = m_max - m_min;
            float const surface_area = 2 * (diff.x() * diff.y() + diff.x() * diff.z() + diff.y() * diff.z());
            return surface_area;
        }

        std::size_t maximum_extent() const noexcept
        {
            gmath::vector<Space> const diff = m_max - m_min;
            if (diff[0] > diff[1] && diff[0] > diff[2])
                return 0;
            if (diff[1] > diff[0] && diff[1] > diff[2])
                return 1;
            return 2;
        }

        gmath::position<Space> const& min() const noexcept { return m_min; }
        gmath::position<Space> const& max() const noexcept { return m_max; }
    private:
        gmath::position<Space> m_min;
        gmath::position<Space> m_max;
    };
}