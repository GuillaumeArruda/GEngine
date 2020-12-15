#pragma once
#include <type_traits>
#include <iterator>

namespace gtl
{
    // lite std::span replacement
    template<class T>
    struct span
    {
        using element_type = T;
        using value_type = std::remove_cv_t<T>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;


        constexpr span() noexcept : m_begin(nullptr), m_end(nullptr) {}
        template<class Iterator>
        constexpr span(Iterator begin, size_type count) noexcept : m_begin(static_cast<pointer>(begin)), m_end(static_cast<pointer>(begin + count)) {}
        template<class BeginIterator, class EndIterator>
        constexpr span(BeginIterator begin, EndIterator end) noexcept : m_begin(static_cast<pointer>(begin)), m_end(static_cast<pointer>(end)) {}
        template<std::size_t N>
        constexpr span(element_type(&arr)[N]) noexcept : m_begin(arr), m_end(arr + N) {}
        template<class U, class = std::enable_if_t<!std::is_same_v<U, T> && std::is_convertible_v<element_type(*)[], U(*)[]>, void>>
        constexpr span(span<U> s) noexcept : m_begin(s.begin()), m_end(s.end()) {}
        template<class Vector, class = std::enable_if_t<std::is_convertible_v<typename Vector::value_type (*)[], element_type(*)[]>, decltype(std::declval<Vector>().data())>>
        constexpr span(Vector const& vec) noexcept : m_begin(vec.data()), m_end(vec.data() + vec.size()) {}

        constexpr span(span const&) noexcept = default;
        constexpr span(span&&) noexcept = default;
        ~span() noexcept = default;

        constexpr span& operator=(span const&) noexcept = default;
        constexpr span& operator=(span&&) noexcept = default;

        constexpr iterator begin() const noexcept { return m_begin; }
        constexpr const_iterator cbegin() const noexcept { return m_begin; }
        constexpr reverse_iterator rbegin() const noexcept { return m_end; }
        constexpr const_reverse_iterator crbegin() const noexcept { return m_end; }

        constexpr iterator end() const noexcept { return m_end; }
        constexpr const_iterator cend() const noexcept { return m_end; }
        constexpr reverse_iterator rend() const noexcept { return m_begin; }
        constexpr const_reverse_iterator crend() const noexcept { return m_begin; }

        constexpr size_type size() const noexcept { return std::distance(m_begin, m_end); }
        constexpr size_type size_bytes() const noexcept { return size() * sizeof(element_type); }
        constexpr bool empty() const noexcept { return size() == 0ull; }

        constexpr pointer data() const noexcept { return m_begin; }
        constexpr reference front() const { return *begin(); }
        constexpr reference back() const { return *(end() - 1); }
        constexpr reference operator[](std::size_t index) const { return data()[index]; }

        constexpr span first(std::size_t count) const { return span(m_begin, count); }
        constexpr span last(std::size_t count) const { return span(m_end - (1 + count), count); }
        constexpr span subspan(std::size_t offset, std::size_t count) const { return span(m_begin + offset, count); }
    private:
        iterator m_begin;
        iterator m_end;
    };
}