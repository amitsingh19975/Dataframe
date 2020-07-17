#if !defined(AMT_CORE_OPERATOR_HPP)
#define AMT_CORE_OPERATOR_HPP

#include <core/type_to_string.hpp>
#include <core/type_traits.hpp>
#include <functional>
#include <iomanip>
#include <ostream>

namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os, Storage auto &&s) {
    if (s.empty())
        return os;
    visit(s, [&os](auto &&val) { os << val; });
    return os;
}

inline constexpr bool operator==(Storage auto &&LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        return false;
    }

    if (LHS.index() != RHS.index()) {
        return false;
    }
    bool f = false;
    visit(LHS, [&RHS, &f](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_comp_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator==(Storage auto&&, Storage auto&&) : "
                "Operator (==) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_comp_equal_v<type, type>) {
            f = (val == rhs);
        }
    });

    return f;
}

inline constexpr bool operator<(Storage auto &&LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        return false;
    }

    if (LHS.index() != RHS.index()) {
        return false;
    }
    bool f = false;
    visit(LHS, [&RHS, &f](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_comp_less_v<type, type>)
            throw std::runtime_error(
                "amt::operator<(Storage auto&&, Storage auto&&) : "
                "Operator (<) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_comp_less_v<type, type>) {
            f = (val < rhs);
        }
    });

    return f;
}

inline constexpr bool operator>(Storage auto &&LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        return false;
    }

    if (LHS.index() != RHS.index()) {
        return false;
    }
    bool f = false;
    visit(LHS, [&RHS, &f](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_comp_greater_v<type, type>)
            throw std::runtime_error(
                "amt::operator>(Storage auto&&, Storage auto&&) : "
                "Operator (>) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_comp_greater_v<type, type>) {
            f = (val > rhs);
        }
    });

    return f;
}

inline constexpr bool operator!=(Storage auto &&LHS, Storage auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr auto operator+(Storage auto &&LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error(
            "amt::operator+(Storage auto&&, Storage auto&&) : "
            "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error(
            "amt::operator+(Storage auto&&, Storage auto&&) : "
            "type mismatch");
    }
    std::decay_t<decltype(LHS)> temp;

    visit(LHS, [&RHS, &temp](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_plus_v<type, type>)
            throw std::runtime_error(
                "amt::operator+(Storage auto&&, Storage auto&&) : "
                "Operator (+) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_op_plus_v<type, type>) {
            temp = val + rhs;
        }
    });

    return temp;
}

inline constexpr auto operator-(Storage auto &&LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error(
            "amt::operator-(Storage auto&&, Storage auto&&) : "
            "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error(
            "amt::operator-(Storage auto&&, Storage auto&&) : "
            "type mismatch");
    }
    std::decay_t<decltype(LHS)> temp;

    visit(LHS, [&RHS, &temp](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_minus_v<type, type>)
            throw std::runtime_error(
                "amt::operator-(Storage auto&&, Storage auto&&) : "
                "Operator (-) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_op_minus_v<type, type>) {
            temp = val - rhs;
        }
    });

    return temp;
}

inline constexpr auto operator*(Storage auto &&LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error(
            "amt::operator*(Storage auto&&, Storage auto&&) : "
            "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error(
            "amt::operator*(Storage auto&&, Storage auto&&) : "
            "type mismatch");
    }
    std::decay_t<decltype(LHS)> temp;

    visit(LHS, [&RHS, &temp](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_mul_v<type, type>)
            throw std::runtime_error(
                "amt::operator*(Storage auto&&, Storage auto&&) : "
                "Operator (*) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_op_mul_v<type, type>) {
            temp = val * rhs;
        }
    });

    return temp;
}

inline constexpr auto operator/(Storage auto &&LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error(
            "amt::operator/(Storage auto&&, Storage auto&&) : "
            "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error(
            "amt::operator/(Storage auto&&, Storage auto&&) : "
            "type mismatch");
    }
    std::decay_t<decltype(LHS)> temp;

    visit(LHS, [&RHS, &temp](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_div_v<type, type>)
            throw std::runtime_error(
                "amt::operator/(Storage auto&&, Storage auto&&) : "
                "Operator (/) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_op_div_v<type, type>) {
            temp = val / rhs;
        }
    });

    return temp;
}

} // namespace amt

namespace amt {

inline constexpr bool operator<=(Storage auto &&LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        return false;
    }

    if (LHS.index() != RHS.index()) {
        return false;
    }
    bool f = false;
    visit(LHS, [&RHS, &f](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_comp_less_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator<=(Storage auto&&, Storage auto&&) : "
                "Operator (<=) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_comp_less_equal_v<type, type>) {
            f = (val <= rhs);
        }
    });

    return f;
}

inline constexpr bool operator>=(Storage auto &&LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        return false;
    }

    if (LHS.index() != RHS.index()) {
        return false;
    }
    bool f = false;
    visit(LHS, [&RHS, &f](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_comp_greater_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator>=(Storage auto&&, Storage auto&&) : "
                "Operator (>=) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_comp_greater_equal_v<type, type>) {
            f = (val >= rhs);
        }
    });

    return f;
}

inline constexpr auto &operator+=(Storage auto &LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error(
            "amt::operator+=(Storage auto&, Storage auto&&) : "
            "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error(
            "amt::operator+=(Storage auto&, Storage auto&&) : "
            "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_plus_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator+=(Storage auto&, Storage auto&&) : "
                "Operator (+=) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_op_plus_equal_v<type, type>) {
            LHS.template cast<type>() += rhs;
        }
    });

    return LHS;
}

inline constexpr auto &operator-=(Storage auto &LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error(
            "amt::operator-=(Storage auto&, Storage auto&&) : "
            "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error(
            "amt::operator-=(Storage auto&, Storage auto&&) : "
            "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_minus_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator-=(Storage auto&, Storage auto&&) : "
                "Operator (-=) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_op_minus_equal_v<type, type>) {
            LHS.template cast<type>() -= rhs;
        }
    });

    return LHS;
}

inline constexpr auto &operator/=(Storage auto &LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error(
            "amt::operator/=(Storage auto&, Storage auto&&) : "
            "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error(
            "amt::operator/=(Storage auto&, Storage auto&&) : "
            "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_div_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator/=(Storage auto&, Storage auto&&) : "
                "Operator (/=) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_op_div_equal_v<type, type>) {
            LHS.template cast<type>() /= rhs;
        }
    });

    return LHS;
}

inline constexpr auto &operator*=(Storage auto &LHS, Storage auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error(
            "amt::operator*=(Storage auto&, Storage auto&&) : "
            "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error(
            "amt::operator*=(Storage auto&, Storage auto&&) : "
            "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_mul_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator*=(Storage auto&, Storage auto&&) : "
                "Operator (*=) is not supported by this type");

        auto const &rhs = RHS.template cast<type>();
        if constexpr (core::has_op_mul_equal_v<type, type>) {
            LHS.template cast<type>() *= rhs;
        }
    });

    return LHS;
}

} // namespace amt

// Series operators
namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os, Series auto &&s) {
    if (s.empty())
        return os << "[]";
    std::size_t i{};
    os << "[ ";
    for (auto const &el : s) {
        (void)(os << el);
        if (i++ < s.size() - 1ul) {
            os << ", ";
        }
    }
    return os << " ]";
}

inline constexpr bool operator==(Series auto &&LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        return false;
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin());
}

inline constexpr bool operator!=(Series auto &&LHS, Series auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr bool operator<(Series auto &&LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator<(Series&&, Series&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(), std::less<>{});
}

inline constexpr bool operator<=(Series auto &&LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator<(Series&&, Series&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(), std::less_equal<>{});
}

inline constexpr bool operator>(Series auto &&LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator<(Series&&, Series&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(), std::greater<>{});
}

inline constexpr bool operator>=(Series auto &&LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator<(Series&&, Series&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(),
                      std::greater_equal<>{});
}

inline auto operator+(Series auto &&LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator+(Series&&, Series&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        temp[i] = LHS[i] + RHS[i];
    }
    return temp;
}

inline auto operator-(Series auto &&LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator-(Series&&, Series&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        temp[i] = LHS[i] - RHS[i];
    }
    return temp;
}

inline auto operator*(Series auto &&LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator*(Series&&, Series&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        temp[i] = LHS[i] * RHS[i];
    }
    return temp;
}

inline auto operator/(Series auto &&LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator/(Series&&, Series&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        temp[i] = LHS[i] / RHS[i];
    }
    return temp;
}

inline auto &operator+=(Series auto &LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator+=(Series&&, Series&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] += RHS[i];
    }
    return LHS;
}

inline auto &operator-=(Series auto &LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator-=(Series&&, Series&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] -= RHS[i];
    }
    return LHS;
}

inline auto &operator*=(Series auto &LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator*=(Series&&, Series&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] *= RHS[i];
    }
    return LHS;
}

inline auto &operator/=(Series auto &LHS, Series auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error("amt::operator/=(Series&&, Series&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] /= RHS[i];
    }
    return LHS;
}

} // namespace amt

// Slice
namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os, Slice auto &&s) {
    return os << "[ Start: " << s.first() << ", End: " << s.last()
              << ", Stride: " << s.step() << " ]";
}

} // namespace amt

// View
namespace amt {

namespace detail {

inline constexpr auto size(View auto &&v) noexcept {
    using ret_type = typename std::decay_t<decltype(v)>::view_of;
    if constexpr (is_frame_v<ret_type>) {
        return v.cols();
    } else {
        return v.size();
    }
}

inline constexpr auto cols(View auto &&v) noexcept {
    using ret_type = typename std::decay_t<decltype(v)>::view_of;
    if constexpr (is_frame_v<ret_type>) {
        return v.cols();
    } else {
        return 1ul;
    }
}

inline constexpr auto rows(View auto &&v) noexcept {
    using ret_type = typename std::decay_t<decltype(v)>::view_of;
    if constexpr (is_frame_v<ret_type>) {
        return v.rows();
    } else {
        return v.size();
    }
}

} // namespace detail

template <View V>
requires Series<
    typename std::decay_t<V>::view_of> inline constexpr std::ostream &
operator<<(std::ostream &os, V &&s) {
    os << "[ ";
    std::size_t i{};
    for (auto const &el : s) {
        os << el;
        if (i < s.size() - 1) {
            os << ", ";
        }
        ++i;
    }
    return os << " ]";
}

template <View V>
requires Frame<
    typename std::decay_t<V>::view_of> inline constexpr std::ostream &
operator<<(std::ostream &os, V &&f) {
    std::size_t i{};
    for (auto const &s : f) {
        os << "[ Column: ";
        os << std::quoted(f.name(i++))
           << ", Type: " << std::quoted(type_to_string(s)) << " ]{ ";
        for (auto j = 0ul; j < f.rows(); ++j) {
            if (s[j].is_string()) {
                os << std::quoted(s[j].template cast<std::string>());
            } else {
                os << s[j];
            }
            if (j < f.rows() - 1ul) {
                os << ", ";
            }
        }
        os << " }\n";
    }
    return os;
}

inline constexpr bool operator==(View auto &&LHS, View auto &&RHS) {
    if (detail::size(LHS) != detail::size(RHS)) {
        return false;
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin());
}

inline constexpr bool operator!=(View auto &&LHS, View auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr bool operator<(View auto &&LHS, View auto &&RHS) {
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator<(View&&, View&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(), std::less<>{});
}

inline constexpr bool operator<=(View auto &&LHS, View auto &&RHS) {
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator<(View&&, View&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(), std::less_equal<>{});
}

inline constexpr bool operator>(View auto &&LHS, View auto &&RHS) {
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator<(View&&, View&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(), std::greater<>{});
}

inline constexpr bool operator>=(View auto &&LHS, View auto &&RHS) {
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator<(View&&, View&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(),
                      std::greater_equal<>{});
}

inline auto operator+(View auto &&LHS, View auto &&RHS) {
    using ret_type = typename std::decay_t<decltype(LHS)>::view_of;
    using size_type = typename ret_type::size_type;

    size_type sz = detail::size(LHS);
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator+(View&&, View&&) : "
                                 "size mismatch");
    }

    ret_type temp(sz);

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < sz; ++i) {
        temp[i] = LHS[i] + RHS[i];
    }
    return temp;
}

inline auto operator-(View auto &&LHS, View auto &&RHS) {
    using ret_type = typename std::decay_t<decltype(LHS)>::view_of;
    using size_type = typename ret_type::size_type;

    size_type sz = detail::size(LHS);
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator-(View&&, View&&) : "
                                 "size mismatch");
    }

    ret_type temp(sz);

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < sz; ++i) {
        temp[i] = LHS[i] - RHS[i];
    }
    return temp;
}

inline auto operator*(View auto &&LHS, View auto &&RHS) {
    using ret_type = typename std::decay_t<decltype(LHS)>::view_of;
    using size_type = typename ret_type::size_type;

    size_type sz = detail::size(LHS);
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator*(View&&, View&&) : "
                                 "size mismatch");
    }

    ret_type temp(sz);

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < sz; ++i) {
        temp[i] = LHS[i] * RHS[i];
    }
    return temp;
}

inline auto operator/(View auto &&LHS, View auto &&RHS) {
    using ret_type = typename std::decay_t<decltype(LHS)>::view_of;
    using size_type = typename ret_type::size_type;

    size_type sz = detail::size(LHS);
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator/(View&&, View&&) : "
                                 "size mismatch");
    }

    ret_type temp(sz);

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < sz; ++i) {
        temp[i] = LHS[i] / RHS[i];
    }
    return temp;
}

template <View VLHS>
requires(VLHS::is_const == false) inline auto &operator+=(VLHS &LHS,
                                                          View auto &&RHS) {
    using ret_type = typename std::decay_t<decltype(LHS)>::view_of;
    using size_type = typename ret_type::size_type;

    size_type sz = detail::size(LHS);
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator+=(View&&, View&&) : "
                                 "size mismatch");
    }

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < sz; ++i) {
        LHS[i] += RHS[i];
    }
    return LHS;
}

template <View VLHS>
requires(VLHS::is_const == false) inline auto &operator-=(VLHS &LHS,
                                                          View auto &&RHS) {
    using ret_type = typename std::decay_t<decltype(LHS)>::view_of;
    using size_type = typename ret_type::size_type;

    size_type sz = detail::size(LHS);
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator-=(View&&, View&&) : "
                                 "size mismatch");
    }

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < sz; ++i) {
        LHS[i] -= RHS[i];
    }
    return LHS;
}

template <View VLHS>
requires(VLHS::is_const == false) inline auto &operator*=(VLHS &LHS,
                                                          View auto &&RHS) {
    using ret_type = typename std::decay_t<decltype(LHS)>::view_of;
    using size_type = typename ret_type::size_type;

    size_type sz = detail::size(LHS);
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator*=(View&&, View&&) : "
                                 "size mismatch");
    }

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < sz; ++i) {
        LHS[i] *= RHS[i];
    }
    return LHS;
}

template <View VLHS>
requires(VLHS::is_const == false) inline auto &operator/=(VLHS &LHS,
                                                          View auto &&RHS) {
    using ret_type = typename std::decay_t<decltype(LHS)>::view_of;
    using size_type = typename ret_type::size_type;

    size_type sz = detail::size(LHS);
    if (detail::size(LHS) != detail::size(RHS)) {
        throw std::runtime_error("amt::operator/=(View&&, View&&) : "
                                 "size mismatch");
    }

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < sz; ++i) {
        LHS[i] /= RHS[i];
    }
    return LHS;
}

} // namespace amt

namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os, Frame auto &&f) {
    if (f.empty())
        return os << "[ Column: Empty, Type: Empty ]{}";

    std::size_t i{};
    for (auto const &s : f) {
        os << "[ Column: ";
        os << std::quoted(f.name(i++))
           << ", Type: " << std::quoted(type_to_string(s)) << " ]{ ";
        for (auto j = 0ul; j < f.rows(); ++j) {
            if (s[j].is_string()) {
                os << std::quoted(s[j].template cast<std::string>());
            } else {
                os << s[j];
            }
            if (j < f.rows() - 1ul) {
                os << ", ";
            }
        }
        os << " }\n";
    }
    return os;
}

inline constexpr bool operator==(Frame auto &&LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        return false;
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin());
}

inline constexpr bool operator!=(Frame auto &&LHS, Frame auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr bool operator<(Frame auto &&LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator<(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(), std::less<>{});
}

inline constexpr bool operator<=(Frame auto &&LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator<(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(), std::less_equal<>{});
}

inline constexpr bool operator>(Frame auto &&LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator<(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(), std::greater<>{});
}

inline constexpr bool operator>=(Frame auto &&LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator<(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    return std::equal(LHS.begin(), LHS.end(), RHS.begin(),
                      std::greater_equal<>{});
}

inline auto operator+(Frame auto &&LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator+(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.cols());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        temp[i] = LHS[i] + RHS[i];
    }
    return temp;
}

inline auto operator-(Frame auto &&LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator-(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.cols());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        temp[i] = LHS[i] - RHS[i];
    }
    return temp;
}

inline auto operator*(Frame auto &&LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator*(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.cols());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        temp[i] = LHS[i] * RHS[i];
    }
    return temp;
}

inline auto operator/(Frame auto &&LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator/(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.cols());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        temp[i] = LHS[i] / RHS[i];
    }
    return temp;
}

inline auto &operator+=(Frame auto &LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator+=(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] += RHS[i];
    }
    return LHS;
}

inline auto &operator-=(Frame auto &LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator-=(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] -= RHS[i];
    }
    return LHS;
}

inline auto &operator*=(Frame auto &LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator*=(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] *= RHS[i];
    }
    return LHS;
}

inline auto &operator/=(Frame auto &LHS, Frame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error("amt::operator/=(Frame&&, Frame&&) : "
                                 "size mismatch");
    }
    using ret_type = std::decay_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] /= RHS[i];
    }
    return LHS;
}

} // namespace amt

#endif // AMT_CORE_OPERATOR_HPP
