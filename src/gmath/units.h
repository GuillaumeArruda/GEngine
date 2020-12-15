#pragma once

#include "gmath/internal/units_helper.h"
#include "gmath/utils.h"

namespace gmath
{
	struct radian
		: helper::unit_friend_op_helper<radian>
	{
		constexpr explicit radian(float value = 0.f) noexcept : m_value(value) {}
		~radian() noexcept = default;

		constexpr radian(radian const&) noexcept = default;
		constexpr radian& operator=(radian const&) noexcept = default;
		constexpr radian(radian&&) noexcept = default;
		constexpr radian& operator=(radian&&) noexcept = default;

		constexpr float value() const noexcept { return m_value; }
		constexpr float& value() noexcept { return m_value; }

		constexpr explicit operator float() const noexcept { return m_value; }
	private:
		float m_value;
	};

	struct degree
		: helper::unit_friend_op_helper<degree>
	{
		static constexpr float radtodegree = 180.f / pi;
		static constexpr float degreetorad = pi / 180.f;
		constexpr explicit degree(float value = 0.f) noexcept : m_value(value) {}
		constexpr explicit degree(radian const& rad) noexcept : m_value(rad.value() * radtodegree) {}
		~degree() noexcept = default;

		constexpr operator radian() const noexcept
		{
			return radian(m_value * degreetorad);
		}

		constexpr degree(degree const&) noexcept = default;
		constexpr degree& operator=(degree const&) noexcept = default;
		constexpr degree(degree&&) noexcept = default;
		constexpr degree& operator=(degree&&) noexcept = default;

		constexpr float value() const noexcept { return m_value; }
		constexpr float& value() noexcept { return m_value; }

		constexpr explicit operator float() const noexcept { return m_value; }
	private:
		float m_value;
	};
}

constexpr gmath::radian operator "" _r(long double value) noexcept
{
	return gmath::radian(static_cast<float>(value));
}

constexpr gmath::degree operator "" _d(long double value) noexcept
{
	return gmath::degree(static_cast<float>(value));
}