/// Mostly borrowed from github.com/PinguimBots/ssl-client.
#pragma once

#include <type_traits>
#include <utility> // For std::forward.
#include <array>

namespace utils
{
    // Extract first type from a parameter pack.
    template <typename... T>
    struct head;
    template <typename Head, typename... Tail>
    struct head<Head, Tail...>{ using type = Head; };

    // When you need to make a std::array of some type but want to
    // decide the size by the amount of arguments.
    template <typename T, typename... U>
    constexpr auto arr(U&&... u)
    {
        return std::array<T, sizeof...(U)>{ std::forward<U>(u)... };
    }
    template <typename... U>
    constexpr auto arr(U&&... u)
    {
        using arr_type = typename head<U...>::type;
        return arr<arr_type, U...>( std::forward<U>(u)... );
    }

    // Compile-time square root.
    // From: https://gist.github.com/kimwalisch/d249cf684a58e1d892e1
    // Thanks!
    constexpr auto sqrt_helper(long long x, long long lo, long long hi) -> long long
    {
      return lo == hi ? lo : ((x / ((lo + hi + 1) / 2) < ((lo + hi + 1) / 2))
          ? sqrt_helper(x, lo, ((lo + hi + 1) / 2) - 1)
          : sqrt_helper(x, ((lo + hi + 1) / 2), hi));
    }

    constexpr auto ct_sqrt(long long x) -> long long
    {
      return sqrt_helper(x, 0, x / 2 + 1);
    }

    // Accesses a 1D array as if it was 2D, assuming X grows to the right and Y grows down.
    template <typename T, std::size_t Size>
    struct bidimensional_access{
        const std::array<T, Size>& original;

        struct span {
            std::size_t x;
            std::size_t y;
        } dims;

        constexpr bidimensional_access(const std::array<T, Size>& original, span dims)
            : original{ original }
            , dims{ dims }
        {}

        constexpr bidimensional_access(const std::array<T, Size>& original)
            : original{ original }
            , dims{ ct_sqrt(Size), ct_sqrt(Size) }
        {}

        constexpr auto operator[](span s) const -> const T& { return original[s.x + s.y * dims.x]; }
    };
}
