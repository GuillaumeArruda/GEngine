#pragma once

#include <glm/vec3.hpp>

#include "gmath/utils.h"
#include "gmath/position.h"
#include "gmath/direction.h"

namespace gmath
{
    template<class Space>
    struct vector
    {
        using space = Space;
        using glm_vector_type = glm::vec3;

        constexpr explicit vector(gmath::no_init_t) noexcept {}
        constexpr explicit vector(glm_vector_type const& vector) noexcept : m_vector(vector) {}
        constexpr explicit vector(position<space> const& position) noexcept : m_vector(position.x(), position.y(), position.z()) {}
        constexpr vector(direction<space> const& dir) noexcept : m_vector(static_cast<glm_vector_type>(dir)) {}
        constexpr vector(float x = 0.f, float y = 0.f, float z = 0.f) noexcept : m_vector(x,y,z) {}
        constexpr vector(vector const&) noexcept = default;
        constexpr vector(vector&&) noexcept = default;
        ~vector() noexcept = default;

        constexpr vector& operator=(vector const&) noexcept = default;
        constexpr vector& operator=(vector&&) noexcept = default;

        constexpr explicit operator glm_vector_type() const noexcept { return m_vector; }
        constexpr explicit operator position<space>() const noexcept{ return gmath::position<space>(m_vector); }
        constexpr operator direction<space>() const noexcept { return direction<space>(m_vector); }

        constexpr vector& operator+=(vector const& rhs) noexcept { m_vector += rhs.m_vector; return *this; }
        friend constexpr vector operator+(vector const& lhs, vector const& rhs) noexcept { return vector(lhs.m_vector + rhs.m_vector); }
        friend constexpr position<space> operator+(vector const& lhs, position<space> const& rhs) noexcept { return position<space>(glm::vec4(lhs.m_vector,0.f) + static_cast<glm::vec4>(rhs)); }
        friend constexpr position<space> operator+(position<space> const& lhs, vector const& rhs) noexcept { return position<space>(static_cast<glm::vec4>(lhs) + glm::vec4(rhs.m_vector, 0.f)); }

        constexpr vector& operator-=(vector const& rhs) noexcept { m_vector -= rhs.m_vector; return *this; }
        friend constexpr vector operator-(vector const& lhs) noexcept { return vector(-lhs.m_vector); }
        friend constexpr vector operator-(vector const& lhs, vector const& rhs) noexcept { return vector(lhs.m_vector - rhs.m_vector); }
        friend constexpr position<space> operator-(vector const& lhs, position<space> const& rhs) noexcept { return position<space>(glm::vec4(lhs.m_vector,0.f) - static_cast<glm::vec4>(rhs)); }
        friend constexpr position<space> operator-(position<space> const& lhs, vector const& rhs) noexcept { return position<space>(static_cast<glm::vec4>(lhs) - glm::vec4(rhs.m_vector,0.f)); }

        constexpr vector& operator*=(float rhs) noexcept { m_vector *= rhs; return *this; }
        friend constexpr vector operator*(vector const& lhs, float rhs) noexcept { return vector(lhs.m_vector * rhs); }
        friend constexpr vector operator*(float lhs, vector const& rhs) noexcept { return vector(lhs * rhs.m_vector); }

        constexpr vector& operator/=(float rhs) noexcept { m_vector /= rhs; return *this; }
        friend constexpr vector operator/(vector const& lhs, float rhs) noexcept { return vector(lhs.m_vector / rhs); }


        friend constexpr bool operator==(vector const& lhs, vector const& rhs) noexcept { return lhs.m_vector == rhs.m_vector; }
        friend constexpr bool operator!=(vector const& lhs, vector const& rhs) noexcept { return !(lhs == rhs); }

        constexpr float length() const noexcept { return glm::length(m_vector); }
        constexpr float length2() const noexcept { return dot(*this); }
        constexpr float distance(vector const& rhs) const noexcept { return glm::distance(m_vector, rhs.m_vector); }
        constexpr float dot(vector const& rhs) const noexcept { return glm::dot(m_vector, rhs.m_vector); }

        constexpr vector cross(vector const& rhs) const noexcept { return vector(glm::cross(m_vector, rhs.m_vector)); }
        constexpr direction<space> normalize() const noexcept { return static_cast<direction<space>>(m_vector); }
        constexpr vector reflect(direction<space> const& normal) const noexcept { return vector(glm::reflect(m_vector, static_cast<glm::vec3>(normal))); }
        constexpr vector refract(direction<space> const& normal, float eta) const noexcept { return vector(glm::refract(m_vector, static_cast<glm::vec3>(normal), eta)); }

        constexpr float& x() noexcept { return m_vector[0]; }
        constexpr float& y() noexcept { return m_vector[1]; }
        constexpr float& z() noexcept { return m_vector[2]; }

        constexpr float x() const noexcept { return m_vector[0]; }
        constexpr float y() const noexcept { return m_vector[1]; }
        constexpr float z() const noexcept { return m_vector[2]; }

        constexpr float operator[](std::size_t index) const noexcept { return m_vector[static_cast<glm_vector_type::length_type>(index)]; }

    private:
        glm_vector_type m_vector;
    };

    template<class Space>
    auto operator-(position<Space> const& lhs, position<Space> const& rhs) noexcept -> vector<Space> 
    { 
        return vector<Space>(static_cast<typename position<Space>::glm_vector_type>(lhs) - static_cast<typename position<Space>::glm_vector_type>(rhs));
    }

    template<class Space>
    bool almost_equal(vector<Space> const& lhs, vector<Space> const& rhs, int ulp = 1) noexcept
    {
        return almost_equal(lhs.x(), rhs.x(), ulp) && almost_equal(lhs.y(), lhs.y(), ulp) && almost_equal(lhs.z(), rhs.z(), ulp);
    }

    template<class Space>
    bool within_epsilon(vector<Space> const& lhs, vector<Space> const& rhs, float epsilon = 1e-6f) noexcept
    {
        return within_epsilon(lhs.x(), rhs.x(), epsilon) && within_epsilon(lhs.y(), rhs.y(), epsilon) && within_epsilon(lhs.z(), rhs.z(), epsilon);
    }

    template<class Space> vector(gmath::vector<Space> const&)->vector<Space>;
    template<class Space> vector(gmath::vector<Space>&&)->vector<Space>;
    template<class Space> vector(gmath::position<Space> const&)->vector<Space>;
    template<class Space> vector(gmath::direction<Space> const&)->vector<Space>;
}