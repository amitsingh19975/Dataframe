#if !defined(AMT_DATAFRAME_CORE_UTILS_HPP)
#define AMT_DATAFRAME_CORE_UTILS_HPP

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <dataframe/traits/basic_traits.hpp>
#include <iomanip>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace amt {

template <typename T>
requires std::is_integral_v<T> consteval std::string_view integer_to_string() {
    if constexpr (sizeof(T) == 2u) {
        return std::numeric_limits<T>::is_signed ? "int16" : "uint16";
    } else if constexpr (sizeof(T) == 4u) {
        return std::numeric_limits<T>::is_signed ? "int32" : "uint32";
    }
    if constexpr (sizeof(T) == 8u) {
        return std::numeric_limits<T>::is_signed ? "int64" : "uint64";
    } else {
        return "Unknown";
    }
}

template <typename T> consteval std::string_view get_type_name_helper() {
    if constexpr (std::is_integral_v<T>) {
        return integer_to_string<T>();
    } else {
        return "Unknown";
    }
}

template <typename T> struct get_type_name {
    static constexpr std::string_view value = get_type_name_helper<T>();
};

template <> struct get_type_name<std::monostate> {
    static constexpr std::string_view value = "any";
};

template <> struct get_type_name<bool> {
    static constexpr std::string_view value = "bool";
};

template <> struct get_type_name<char> {
    static constexpr std::string_view value = "char";
};

template <> struct get_type_name<unsigned char> {
    static constexpr std::string_view value = "uchar";
};

template <> struct get_type_name<float> {
    static constexpr std::string_view value = "float";
};

template <> struct get_type_name<double> {
    static constexpr std::string_view value = "double";
};

template <> struct get_type_name<std::string> {
    static constexpr std::string_view value = "std::string";
};

template <typename T>
inline static constexpr std::string_view get_type_name_v =
    get_type_name<T>::value;

template <typename T>
requires(!std::is_constructible_v<std::string, T>) constexpr T &beautify(
    T &val) noexcept {
    return val;
}

auto beautify(std::string_view val) { return std::quoted(val.data()); }

std::string beautify(char val) {
    return std::string("\'") + val + std::string("\'");
}

std::string beautify(bool val) { return val ? "true" : "false"; }

template <typename InputIter1, typename InputIter2, typename OutputIter,
          typename Fn>
requires std::is_same_v<
    std::decay_t<typename InputIter1::value_type>,
    std::decay_t<typename InputIter2::value_type>> constexpr void
parallel_transform(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                   OutputIter out, Fn &&fn) {
#pragma omp parallel for schedule(static, 1)
    for (; first1 != last1; ++first1, ++out, ++first2) {
        *out = std::invoke(std::forward<Fn>(fn), *first1, *first2);
    }
}

template <typename InputIter, typename OutputIter, typename Fn>
constexpr void parallel_transform(InputIter first, InputIter last,
                                  OutputIter out, Fn &&fn) {
#pragma omp parallel for schedule(static, 1)
    for (; first != last; ++first, ++out) {
        *out = std::invoke(std::forward<Fn>(fn), *first);
    }
}

template <typename InputIter, typename OutputIter>
constexpr void parallel_copy(InputIter first, InputIter last, OutputIter out) {
#pragma omp parallel for schedule(static, 1)
    for (; first != last; ++first, ++out) {
        *out = *first;
    }
}

template <typename InputIter, typename OutputIter>
constexpr void parallel_move(InputIter first, InputIter last, OutputIter out) {
#pragma omp parallel for schedule(static, 1)
    for (; first != last; ++first, ++out) {
        *out = std::move(*first);
    }
}

using index_list = std::vector<std::size_t>;
using name_list = std::vector<std::string_view>;

template <typename T>
inline static constexpr bool is_index_list_v =
    std::is_same_v<std::decay_t<T>, index_list>;

template <typename T>
inline static constexpr bool is_name_list_v =
    std::is_same_v<std::decay_t<T>, name_list>;

inline constexpr void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](auto ch) { return !std::isspace(ch); }));
}

inline constexpr void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](auto ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

inline constexpr void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

constexpr index_list &normalize_list(index_list &l, std::size_t max_idx) {
    auto it = std::unique(l.begin(), l.end());
    l.erase(it, l.end());
    it = std::remove_if(l.begin(), l.end(),
                        [sz = max_idx](auto i) { return i >= sz; });
    l.erase(it, l.end());
    std::sort(l.begin(), l.end());
    return l;
}

namespace detail {
template <typename TupleTraits, typename Fn, std::size_t... Is>
requires is_tuple_v<TupleTraits> constexpr auto
tuple_for_helper(TupleTraits &&t, Fn &&fn,
                 [[maybe_unused]] std::index_sequence<Is...> idxs) noexcept {
    constexpr auto sz = std::tuple_size_v<std::decay_t<TupleTraits>>;
    if constexpr (sz > 0u) {
        using first_element_type = decltype(std::get<0>(t));
        using in = std::integral_constant<std::size_t, 0u>;
        if constexpr (std::is_invocable_v<Fn, first_element_type, in>) {
            ((void)std::invoke(std::forward<Fn>(fn), std::get<Is>(t),
                               std::integral_constant<std::size_t, Is>{}),
             ...);

        } else {
            ((void)std::invoke(std::forward<Fn>(fn), std::get<Is>(t)), ...);
        }
    }
}
template <typename Fn, std::size_t... Is>
constexpr auto
tuple_for_helper(Fn &&fn,
                 [[maybe_unused]] std::index_sequence<Is...> idxs) noexcept {
    ((void)std::invoke(std::forward<Fn>(fn),
                       std::integral_constant<std::size_t, Is>{}),
     ...);
}
} // namespace detail

template <typename TupleTraits, typename Fn>
requires is_tuple_v<TupleTraits> constexpr auto tuple_for(TupleTraits &&t,
                                                          Fn &&fn) noexcept {
    detail::tuple_for_helper(
        std::forward<TupleTraits>(t), std::forward<Fn>(fn),
        std::make_index_sequence<
            std::tuple_size_v<std::decay_t<TupleTraits>>>{});
}

template <typename TupleTraits, typename Fn>
requires is_tuple_v<TupleTraits> constexpr auto tuple_for(Fn &&fn) noexcept {
    detail::tuple_for_helper(
        std::forward<Fn>(fn),
        std::make_index_sequence<
            std::tuple_size_v<std::decay_t<TupleTraits>>>{});
}

namespace conv {

std::optional<float> to_float(std::string_view s) {
    char *end;
    auto res = std::strtof(s.data(), &end);
    auto err = (errno == ERANGE);
    errno = 0;
    if (end == s.data() || *end != '\0' || err)
        return std::nullopt;
    return {res};
}

std::optional<double> to_double(std::string_view s) {
    char *end;
    auto res = std::strtod(s.data(), &end);
    auto err = (errno == ERANGE);
    errno = 0;
    if (end == s.data() || *end != '\0' || err)
        return std::nullopt;
    return {res};
}

std::optional<long double> to_long_double(std::string_view s) {
    char *end;
    auto res = std::strtold(s.data(), &end);
    auto err = (errno == ERANGE);
    errno = 0;
    if (end == s.data() || *end != '\0' || err)
        return std::nullopt;
    return {res};
}

std::optional<long> to_long(std::string_view s) {
    char *end;
    auto res = std::strtol(s.data(), &end, 10);
    auto err = (errno == ERANGE);
    errno = 0;
    if (end == s.data() || *end != '\0' || err)
        return std::nullopt;
    return {res};
}

std::optional<long long> to_long_long(std::string_view s) {
    char *end;
    auto res = std::strtoll(s.data(), &end, 10);
    auto err = (errno == ERANGE);
    errno = 0;
    if (end == s.data() || *end != '\0' || err)
        return std::nullopt;
    return {res};
}

std::optional<unsigned long> to_unsigned_long(std::string_view s) {
    char *end;
    auto res = std::strtoul(s.data(), &end, 10);
    auto err = (errno == ERANGE);
    errno = 0;
    if (end == s.data() || *end != '\0' || err)
        return std::nullopt;
    return {res};
}

std::optional<unsigned long long> to_unsigned_long_long(std::string_view s) {
    char *end;
    auto res = std::strtoull(s.data(), &end, 10);
    auto err = (errno == ERANGE);
    errno = 0;
    if (end == s.data() || *end != '\0' || err)
        return std::nullopt;
    return {res};
}

bool cast(float &res, std::string_view val) {
    auto op = to_float(val);

    if (op)
        res = *op;
    else
        return false;

    return true;
}

bool cast(double &res, std::string_view val) {
    auto op = to_double(val);

    if (op)
        res = *op;
    else
        return false;

    return true;
}

bool cast(long double &res, std::string_view val) {
    auto op = to_long_double(val);

    if (op)
        res = *op;
    else
        return false;

    return true;
}

bool cast(long &res, std::string_view val) {
    auto op = to_long(val);

    if (op)
        res = *op;
    else
        return false;

    return true;
}

bool cast(unsigned long &res, std::string_view val) {
    auto op = to_unsigned_long(val);

    if (op)
        res = *op;
    else
        return false;

    return true;
}

bool cast(long long &res, std::string_view val) {
    auto op = to_long_long(val);
    if (op)
        res = *op;
    else
        return false;

    return true;
}

bool cast(unsigned long long &res, std::string_view val) {
    auto op = to_unsigned_long_long(val);

    if (op)
        res = *op;
    else
        return false;

    return true;
}

template <typename T>
requires(std::is_integral_v<T> &&std::numeric_limits<T>::is_signed) bool cast(
    T &res, std::string_view val) {
    auto op = to_long_long(val);

    if (op)
        res = static_cast<T>(*op);
    else
        return false;

    return true;
}

template <typename T>
requires(std::is_integral_v<T> &&
         !std::numeric_limits<T>::is_signed) bool cast(T &res,
                                                       std::string_view val) {
    auto op = to_unsigned_long_long(val);

    if (op)
        res = static_cast<T>(*op);
    else
        return false;

    return true;
}

bool cast(bool &res, std::string_view val) {
    if (val.size() > 5u)
        return false;

    constexpr std::string_view lookup[2] = {"true", "false"};

    if (val.size() == 1u) {
        if (val[0] == '0' || std::tolower(val[0]) == 'f') {
            res = false;
            return true;
        } else if (val[0] == '1' || std::tolower(val[0]) == 't') {
            res = true;
            return true;
        }
        return false;
    } else if (val.size() == 4u) {
        for (auto i = 0u; i < lookup[0].size(); ++i) {
            if (std::tolower(val[i]) != lookup[0][i]) {
                return false;
            }
        }
        res = true;
    } else if (val.size() == 5u) {
        for (auto i = 0u; i < lookup[1].size(); ++i) {
            if (std::tolower(val[i]) != lookup[1][i]) {
                return false;
            }
        }
        res = false;
    }
    return true;
}

bool cast(char &res, std::string_view val) {
    if (val.size() > 1u)
        return false;

    res = val[0];
    return true;
}

bool cast(unsigned char &res, std::string_view val) {
    if (val.size() > 1u)
        return false;

    res = static_cast<unsigned char>(val[0]);
    return true;
}

bool cast(std::string &res, std::string val) {
    res = std::move(val);
    return true;
}

template <HasStdToString T> bool cast(std::string &res, T &&val) {
    res = std::to_string(std::forward<T>(val));
    return true;
}

template <typename T> bool cast(T &res, char val) {
    return cast(res, std::string(1u, val));
}

template <typename T, typename U>
requires(std::is_convertible_v<T, U>) bool cast(T &res, U &&val) {
    res = static_cast<T>(std::forward<U>(val));
    return true;
}

template <typename To, typename From> concept HasCast = requires(To t, From f) {
    {cast(t, f)};
};

} // namespace conv

} // namespace amt

#endif // AMT_DATAFRAME_CORE_UTILS_HPP
