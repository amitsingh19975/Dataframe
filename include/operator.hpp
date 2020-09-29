#if !defined(AMT_CORE_OPERATOR_HPP)
#define AMT_CORE_OPERATOR_HPP

#include <core/type_to_string.hpp>
#include <core/type_traits.hpp>
#include <functional>
#include <iomanip>
#include <ostream>

namespace amt::detail {

template <typename T> inline constexpr auto size(T &&v) noexcept {
    using ret_type = std::decay_t<T>;
    if constexpr (is_frame_v<ret_type> || is_frame_view_v<T>) {
        return v.cols();
    } else {
        return v.size();
    }
}

template <typename T> inline constexpr auto cols(T &&v) noexcept {
    using ret_type = std::decay_t<T>;
    if constexpr (is_frame_v<ret_type> || is_frame_view_v<T>) {
        return v.cols();
    } else {
        return 1ul;
    }
}

template <typename T> inline constexpr auto rows(T &&v) noexcept {
    using ret_type = std::decay_t<T>;
    if constexpr (is_frame_v<ret_type> || is_frame_view_v<T>) {
        return v.rows();
    } else {
        return v.size();
    }
}

} // namespace amt::detail

namespace amt::core {

template <typename Container1, typename Container2, typename BinaryFn>
requires(FrameViewOrFrame<Container1> ||
         SeriesViewOrSeries<
             Container2>) inline constexpr bool equal(Container1 const &LHS,
                                                      Container2 const &RHS,
                                                      BinaryFn &&fn) noexcept {
    std::size_t sz = ::amt::detail::size(LHS);
#pragma omp parallel for schedule(static)
    for (auto i = 0ul; i < sz; ++i) {
        if (!fn(LHS[i], RHS[i]))
            return false;
    }
    return true;
}

} // namespace amt::core

namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os, Box auto &&s) {
    if (s.empty())
        return os;
    visit(s, [&os](auto &&val) { os << val; });
    return os;
}

inline constexpr bool operator==(Box auto &&LHS, Box auto &&RHS) {
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
                "amt::operator==(Box auto&&, Box auto&&) : "
                "Operator (==) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_comp_equal_v<type, type>) {
            f = (val == rhs);
        }
    });

    return f;
}

inline constexpr bool operator<(Box auto &&LHS, Box auto &&RHS) {
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
                "amt::operator<(Box auto&&, Box auto&&) : "
                "Operator (<) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_comp_less_v<type, type>) {
            f = (val < rhs);
        }
    });

    return f;
}

inline constexpr bool operator>(Box auto &&LHS, Box auto &&RHS) {
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
                "amt::operator>(Box auto&&, Box auto&&) : "
                "Operator (>) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_comp_greater_v<type, type>) {
            f = (val > rhs);
        }
    });

    return f;
}

inline constexpr bool operator!=(Box auto &&LHS, Box auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr auto operator+(Box auto &&LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator+(Box auto&&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator+(Box auto&&, Box auto&&) : "
                                 "type mismatch");
    }
    std::decay_t<decltype(LHS)> temp;

    visit(LHS, [&RHS, &temp](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_plus_v<type, type>)
            throw std::runtime_error(
                "amt::operator+(Box auto&&, Box auto&&) : "
                "Operator (+) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_op_plus_v<type, type>) {
            temp = val + rhs;
        }
    });

    return temp;
}

inline constexpr auto operator-(Box auto &&LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator-(Box auto&&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator-(Box auto&&, Box auto&&) : "
                                 "type mismatch");
    }
    std::decay_t<decltype(LHS)> temp;

    visit(LHS, [&RHS, &temp](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_minus_v<type, type>)
            throw std::runtime_error(
                "amt::operator-(Box auto&&, Box auto&&) : "
                "Operator (-) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_op_minus_v<type, type>) {
            temp = val - rhs;
        }
    });

    return temp;
}

inline constexpr auto operator*(Box auto &&LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator*(Box auto&&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator*(Box auto&&, Box auto&&) : "
                                 "type mismatch");
    }
    std::decay_t<decltype(LHS)> temp;

    visit(LHS, [&RHS, &temp](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_mul_v<type, type>)
            throw std::runtime_error(
                "amt::operator*(Box auto&&, Box auto&&) : "
                "Operator (*) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_op_mul_v<type, type>) {
            temp = val * rhs;
        }
    });

    return temp;
}

inline constexpr auto operator/(Box auto &&LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator/(Box auto&&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator/(Box auto&&, Box auto&&) : "
                                 "type mismatch");
    }
    std::decay_t<decltype(LHS)> temp;

    visit(LHS, [&RHS, &temp](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_div_v<type, type>)
            throw std::runtime_error(
                "amt::operator/(Box auto&&, Box auto&&) : "
                "Operator (/) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_op_div_v<type, type>) {
            temp = val / rhs;
        }
    });

    return temp;
}

} // namespace amt

namespace amt {

inline constexpr bool operator<=(Box auto &&LHS, Box auto &&RHS) {
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
                "amt::operator<=(Box auto&&, Box auto&&) : "
                "Operator (<=) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_comp_less_equal_v<type, type>) {
            f = (val <= rhs);
        }
    });

    return f;
}

inline constexpr bool operator>=(Box auto &&LHS, Box auto &&RHS) {
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
                "amt::operator>=(Box auto&&, Box auto&&) : "
                "Operator (>=) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_comp_greater_equal_v<type, type>) {
            f = (val >= rhs);
        }
    });

    return f;
}

inline constexpr auto &operator+=(Box auto &LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator+=(Box auto&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator+=(Box auto&, Box auto&&) : "
                                 "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_plus_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator+=(Box auto&, Box auto&&) : "
                "Operator (+=) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_op_plus_equal_v<type, type>) {
            LHS.template as<type>() += rhs;
        }
    });

    return LHS;
}

inline constexpr auto &operator-=(Box auto &LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator-=(Box auto&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator-=(Box auto&, Box auto&&) : "
                                 "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_minus_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator-=(Box auto&, Box auto&&) : "
                "Operator (-=) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_op_minus_equal_v<type, type>) {
            LHS.template as<type>() -= rhs;
        }
    });

    return LHS;
}

inline constexpr auto &operator/=(Box auto &LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator/=(Box auto&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator/=(Box auto&, Box auto&&) : "
                                 "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_div_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator/=(Box auto&, Box auto&&) : "
                "Operator (/=) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_op_div_equal_v<type, type>) {
            LHS.template as<type>() /= rhs;
        }
    });

    return LHS;
}

inline constexpr auto &operator*=(Box auto &LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator*=(Box auto&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator*=(Box auto&, Box auto&&) : "
                                 "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        if (!core::has_op_mul_equal_v<type, type>)
            throw std::runtime_error(
                "amt::operator*=(Box auto&, Box auto&&) : "
                "Operator (*=) is not supported by this type");

        auto const &rhs = RHS.template as<type>();
        if constexpr (core::has_op_mul_equal_v<type, type>) {
            LHS.template as<type>() *= rhs;
        }
    });

    return LHS;
}

} // namespace amt

// SeriesViewOrSeries operators
namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os,
                                          SeriesViewOrSeries auto &&s) {
    if (s.empty())
        return os << "( Col: \"\", Type: None )[]";
    std::size_t i{};
    os << "( Col: " << std::quoted(s.name()) << ", Type: " << type_to_string(s)
       << " )[ ";
    for (auto const &el : s) {
        if (el.is_string()) {
            os << std::quoted(el.template as<std::string>());
        } else if (el.is_char()) {
            os << "'" << el << "'";
        } else {
            os << el;
        }
        if (i++ < s.size() - 1ul) {
            os << ", ";
        }
    }
    return os << " ]";
}

inline constexpr bool operator==(SeriesViewOrSeries auto &&LHS,
                                 SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        return false;
    }
    return core::equal(LHS, RHS,
                       [](auto const &l, auto const &r) { return l == r; });
}

inline constexpr bool operator!=(SeriesViewOrSeries auto &&LHS,
                                 SeriesViewOrSeries auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr bool operator<(SeriesViewOrSeries auto &&LHS,
                                SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator<(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    return core::equal(LHS, RHS, std::less<>{});
}

inline constexpr bool operator<=(SeriesViewOrSeries auto &&LHS,
                                 SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator<(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    return core::equal(LHS, RHS, std::less_equal<>{});
}

inline constexpr bool operator>(SeriesViewOrSeries auto &&LHS,
                                SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator<(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    return core::equal(LHS, RHS, std::greater<>{});
}

inline constexpr bool operator>=(SeriesViewOrSeries auto &&LHS,
                                 SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator<(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    return core::equal(LHS, RHS, std::greater_equal<>{});
}

inline auto operator+(SeriesViewOrSeries auto &&LHS,
                      SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator+(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        temp[i] = LHS[i] + RHS[i];
    }
    return temp;
}

inline auto operator-(SeriesViewOrSeries auto &&LHS,
                      SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator-(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        temp[i] = LHS[i] - RHS[i];
    }
    return temp;
}

inline auto operator*(SeriesViewOrSeries auto &&LHS,
                      SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator*(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        temp[i] = LHS[i] * RHS[i];
    }
    return temp;
}

inline auto operator/(SeriesViewOrSeries auto &&LHS,
                      SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator/(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        temp[i] = LHS[i] / RHS[i];
    }
    return temp;
}

inline auto &operator+=(SeriesViewOrSeries auto &LHS,
                        SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator+=(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] += RHS[i];
    }
    return LHS;
}

inline auto &operator-=(SeriesViewOrSeries auto &LHS,
                        SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator-=(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] -= RHS[i];
    }
    return LHS;
}

inline auto &operator*=(SeriesViewOrSeries auto &LHS,
                        SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator*=(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] *= RHS[i];
    }
    return LHS;
}

inline auto &operator/=(SeriesViewOrSeries auto &LHS,
                        SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator/=(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
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

namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os,
                                          FrameViewOrFrame auto &&f) {
    if (f.empty())
        return os << "{}";

    os << "{ \n";
    auto sp = "    ";
    for (auto const &s : f) {
        os<<sp<<s<<'\n';
    }
    os << "}\n";
    return os;
}

inline constexpr bool operator==(FrameViewOrFrame auto &&LHS,
                                 FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        return false;
    }
    return core::equal(LHS, RHS,
                       [](auto const &l, auto const &r) { return l == r; });
}

inline constexpr bool operator!=(FrameViewOrFrame auto &&LHS,
                                 FrameViewOrFrame auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr bool operator<(FrameViewOrFrame auto &&LHS,
                                FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator<(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    return core::equal(LHS, RHS, std::less<>{});
}

inline constexpr bool operator<=(FrameViewOrFrame auto &&LHS,
                                 FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator<(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    return core::equal(LHS, RHS, std::less_equal<>{});
}

inline constexpr bool operator>(FrameViewOrFrame auto &&LHS,
                                FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator<(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    return core::equal(LHS, RHS, std::greater<>{});
}

inline constexpr bool operator>=(FrameViewOrFrame auto &&LHS,
                                 FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator<(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    return core::equal(LHS, RHS, std::greater_equal<>{});
}

inline auto operator+(FrameViewOrFrame auto &&LHS,
                      FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator+(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.cols());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        temp[i] = LHS[i] + RHS[i];
    }
    return temp;
}

inline auto operator-(FrameViewOrFrame auto &&LHS,
                      FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator-(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.cols());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        temp[i] = LHS[i] - RHS[i];
    }
    return temp;
}

inline auto operator*(FrameViewOrFrame auto &&LHS,
                      FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator*(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.cols());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        temp[i] = LHS[i] * RHS[i];
    }
    return temp;
}

inline auto operator/(FrameViewOrFrame auto &&LHS,
                      FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator/(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.cols());

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        temp[i] = LHS[i] / RHS[i];
    }
    return temp;
}

inline auto &operator+=(FrameViewOrFrame auto &LHS,
                        FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator+=(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] += RHS[i];
    }
    return LHS;
}

inline auto &operator-=(FrameViewOrFrame auto &LHS,
                        FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator-=(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] -= RHS[i];
    }
    return LHS;
}

inline auto &operator*=(FrameViewOrFrame auto &LHS,
                        FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator*=(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] *= RHS[i];
    }
    return LHS;
}

inline auto &operator/=(FrameViewOrFrame auto &LHS,
                        FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator/=(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] /= RHS[i];
    }
    return LHS;
}

} // namespace amt

#endif // AMT_CORE_OPERATOR_HPP
