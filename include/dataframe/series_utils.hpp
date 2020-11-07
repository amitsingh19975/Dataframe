#if !defined(AMT_DATAFRAME_SERIES_UTILS_HPP)
#define AMT_DATAFRAME_SERIES_UTILS_HPP

#include <dataframe/box_utils.hpp>
#include <dataframe/core/utils.hpp>
#include <dataframe/traits/series_traits.hpp>

namespace amt {

constexpr bool is_mixed_types(Series auto &&s) noexcept {
    return s.dtype() == 0u;
}

std::string type_to_string(Series auto const &s) {
    if (!is_mixed_types(s)) {
        if(s.empty()) return "none";
        return type_to_string(s.back());
    } else {
        return "mixed";
    }
}

template<typename T>
constexpr bool is(Series auto&& s) noexcept{
    if (!is_mixed_types(s)) {
        if(s.empty()) return false;
        return is<T>(s.back());
    } else {
        return is_monostate_v<T>;
    }
}

template <typename... Ts> constexpr bool holds_type(Series auto &&s) noexcept {
    return ( is<Ts>(s) || ... || false );
}

template <typename... Ts> constexpr bool any_holds_type(Series auto &&s) noexcept {
    for (auto const &el : s) {
        if (!(is<Ts>(el) || ... || false))
            return false;
    }
    return true;
}

constexpr bool is_integer(Series auto const &s) noexcept {
    return holds_type<short int, int, long int, long long int,
                      unsigned short int, unsigned int, unsigned long int,
                      unsigned long long int, std::size_t, std::ptrdiff_t>(s);
}

constexpr bool is_char(Series auto const &s) noexcept {
    return holds_type<char, unsigned char>(s);
}

constexpr bool is_bool(Series auto const &s) noexcept {
    return holds_type<bool>(s);
}

constexpr bool is_floating_point(Series auto const &s) noexcept {
    return holds_type<float, double>(s);
}

constexpr bool is_string(Series auto const &s) noexcept {
    return holds_type<std::string>(s);
}

namespace detail {
constexpr void set_dtype(Series auto &s, DType auto d) noexcept {
    using box_type = typename std::decay_t<decltype(s)>::value_type;
    s.m_dtype = get<box_type>(d);
}
} // namespace detail

} // namespace amt

#endif // AMT_DATAFRAME_SERIES_UTILS_HPP
