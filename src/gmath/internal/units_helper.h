#pragma once
namespace gmath
{
	namespace helper
	{
		template<class T>
		struct unit_friend_op_helper
		{
			constexpr T operator-() const noexcept
			{
				return T(-selff().value());
			}

			constexpr T& operator+=(T const& rhs) noexcept
			{
				T& self = selff();
				self.value() += rhs.value();
				return self;
			}

			constexpr T& operator-=(T const& rhs) noexcept
			{
				T& self = selff();
				self.value() -= rhs.value();
				return self;
			}

			constexpr T& operator/=(T const& rhs) noexcept
			{
				T& self = selff();
				self.value() /= rhs.value();
				return self;
			}

			constexpr T& operator*=(T const& rhs) noexcept
			{
				T& self = selff();
				self.value() *= rhs.value();
				return self;
			}

			friend constexpr T operator+(T const& lhs, T const& rhs) noexcept
			{
				T result(lhs);
				return result += rhs;
			}

			friend constexpr T operator-(T const& lhs, T const& rhs) noexcept
			{
				T result(lhs);
				return result -= rhs;
			}

			friend constexpr T operator/(T const& lhs, T const& rhs) noexcept
			{
				T result(lhs);
				return result /= rhs;
			}

			friend constexpr T operator*(T const& lhs, T const& rhs) noexcept
			{
				T result(lhs);
				return result *= rhs;
			}

			friend constexpr bool operator==(T const& lhs, T const& rhs) noexcept
			{
				return lhs.value() == rhs.value();
			}

			friend constexpr bool operator!=(T const& lhs, T const& rhs) noexcept
			{
				return !(lhs == rhs);
			}

			friend constexpr bool operator<(T const& lhs, T const& rhs) noexcept
			{
				return lhs.value() < rhs.value();
			}
			
			friend constexpr bool operator>(T const& lhs, T const& rhs) noexcept
			{
				return lhs.value() > rhs.value();
			}

			friend constexpr bool operator<=(T const& lhs, T const& rhs) noexcept
			{
				return lhs.value() <= rhs.value();
			}

			friend bool constexpr operator>=(T const& lhs, T const& rhs) noexcept
			{
				return lhs.value() >= rhs.value();
			}

			friend constexpr bool almost_equal(T const& lhs, T const& rhs, int ulp = 1) noexcept
			{
				return almost_equal(lhs.value(), rhs.value(), ulp);
			}

			friend constexpr bool within_epsilon(T const& lhs, T const& rhs, float epsilon =  1e-6f ) noexcept
			{
				return within_epsilon(lhs.value(), rhs.value(), epsilon);
			}
		private:
			constexpr T& selff() noexcept { return static_cast<T&>(*this); }
			constexpr const T& selff() const noexcept { return static_cast<const T&>(*this); }
		};
	}
}