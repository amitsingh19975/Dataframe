#if !defined(AMT_DATAFRAME_FUNCTIONS_BOX_MATHS_HPP)
#define AMT_DATAFRAME_FUNCTIONS_BOX_MATHS_HPP

#include <box_utils.hpp>
#include <cmath>
#include <core/macro.hpp>
#include <type_traits.hpp>

namespace amt::impl{

namespace detail {
template <typename Fn>
constexpr void apply_fn(Box auto const &b, Fn &&fn) noexcept {
    visit(b, [fn = std::forward<Fn>(fn)]<typename T>(T const &val) {
        if constexpr (std::is_floating_point_v<T> || std::is_integral_v<T>) {
            std::invoke(fn, val);
        }
    });
}

template <typename Fn>
constexpr void apply_fn(Box auto const &x, Box auto const &y,
                        Fn &&fn) noexcept {
    visit(x, [fn = std::forward<Fn>(fn), &y]<typename T>(T const &val) {
        if constexpr (std::is_floating_point_v<T> || std::is_integral_v<T>) {
            visit(y,
                  [fn = std::move(fn), xval = val]<typename U>(U const &yval) {
                      if constexpr (std::is_floating_point_v<U> ||
                                    std::is_integral_v<U>) {
                          std::invoke(fn, xval, yval);
                      }
                  });
        }
    });
}

template <typename U, typename Fn>
requires(!Box<U> &&
         (std::is_floating_point_v<U> ||
          std::is_integral_v<U>)) constexpr void apply_fn(Box auto const &x,
                                                          U const &y,
                                                          Fn &&fn) noexcept {
    visit(x, [fn = std::forward<Fn>(fn), &y]<typename T>(T const &val) {
        if constexpr (std::is_floating_point_v<T> || std::is_integral_v<T>) {
            std::invoke(fn, val, y);
        }
    });
}

template <typename U, typename Fn>
requires(!Box<U> &&
         (std::is_floating_point_v<U> ||
          std::is_integral_v<U>)) constexpr void apply_fn(U const &x,
                                                          Box auto const &y,
                                                          Fn &&fn) noexcept {
    visit(y, [fn = std::forward<Fn>(fn), &x]<typename T>(T const &val) {
        if constexpr (std::is_floating_point_v<T> || std::is_integral_v<T>) {
            std::invoke(fn, x, val);
        }
    });
}

} // namespace detail

template <Box BoxType> constexpr BoxType abs(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(b, [&ret]<typename T>(T const &val) {
        ret = val;
        constexpr auto is_signed = std::numeric_limits<T>::is_signed;
        if constexpr (is_signed) {
            ret = (val < T{}) ? -val : val;
        }
    });
    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::abs(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType exp(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(b,
                     [&ret]<typename T>(T const &val) { ret = std::exp(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::exp(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType exp2(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::exp2(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::exp2(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType log(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(b,
                     [&ret]<typename T>(T const &val) { ret = std::log(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::log(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType log10(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::log10(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::log10(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType log2(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::log2(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::log2(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType sqrt(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::sqrt(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::sqrt(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType cbrt(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::cbrt(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::cbrt(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType sin(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(b,
                     [&ret]<typename T>(T const &val) { ret = std::sin(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::sin(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType cos(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(b,
                     [&ret]<typename T>(T const &val) { ret = std::cos(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::cos(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType tan(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(b,
                     [&ret]<typename T>(T const &val) { ret = std::tan(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::tan(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType asin(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::asin(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::asin(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType acos(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::acos(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::acos(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType atan(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::atan(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::atan(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType sinh(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::sinh(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::sinh(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType cosh(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::cosh(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::cosh(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType tanh(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::tanh(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::tanh(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType asinh(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::asinh(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::asinh(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType acosh(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::acosh(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::acosh(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType atanh(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::atanh(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::atanh(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType ceil(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::ceil(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::ceil(BoxType const&) : type must be either floating "
                     "point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType floor(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::floor(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::floor(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType round(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::round(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::round(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType trunc(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::trunc(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::trunc(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType isfinite(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::isfinite(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::isfinite(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType isinf(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::isinf(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::isinf(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType isnan(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::isnan(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::isnan(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType isnormal(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::isnormal(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::isnormal(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <Box BoxType> constexpr BoxType signbit(BoxType const &b) {
    BoxType ret;
    detail::apply_fn(
        b, [&ret]<typename T>(T const &val) { ret = std::signbit(val); });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::signbit(BoxType const&) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <typename T, typename U>
constexpr box_result_t<T, U> pow(T const &b, U const &y) {
    box_result_t<T, U> ret;
    detail::apply_fn(b, y, [&ret](auto const &xval, auto const &yval) {
        ret = std::pow(xval, yval);
    });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::pow(T const&, U const& y) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <typename T, typename U>
constexpr box_result_t<T, U> isgreater(T const &b, U const &y) {
    box_result_t<T, U> ret;
    detail::apply_fn(b, y, [&ret](auto const &xval, auto const &yval) {
        ret = std::isgreater(xval, yval);
    });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::isgreater(T const&, U const& y) : type must be "
                     "either floating point or integral"));
    }
    return ret;
}

template <typename T, typename U>
constexpr box_result_t<T, U> isgreaterequal(T const &b, U const &y) {
    box_result_t<T, U> ret;
    detail::apply_fn(b, y, [&ret](auto const &xval, auto const &yval) {
        ret = std::isgreaterequal(xval, yval);
    });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::isgreaterequal(T const&, U const& y) : type must be "
                     "either floating point or integral"));
    }
    return ret;
}

template <typename T, typename U>
constexpr box_result_t<T, U> isless(T const &b, U const &y) {
    box_result_t<T, U> ret;
    detail::apply_fn(b, y, [&ret](auto const &xval, auto const &yval) {
        ret = std::isless(xval, yval);
    });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::isless(T const&, U const& y) : type must be either "
                     "floating point or integral"));
    }
    return ret;
}

template <typename T, typename U>
constexpr box_result_t<T, U> islessequal(T const &b, U const &y) {
    box_result_t<T, U> ret;
    detail::apply_fn(b, y, [&ret](auto const &xval, auto const &yval) {
        ret = std::islessequal(xval, yval);
    });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::islessequal(T const&, U const& y) : type must be "
                     "either floating point or integral"));
    }
    return ret;
}

template <typename T, typename U>
constexpr box_result_t<T, U> islessgreater(T const &b, U const &y) {
    box_result_t<T, U> ret;
    detail::apply_fn(b, y, [&ret](auto const &xval, auto const &yval) {
        ret = std::islessgreater(xval, yval);
    });

    if (ret.empty()) {
        throw std::runtime_error(
            ERR_CSTR("amt::islessgreater(T const&, U const& y) : type must be "
                     "either floating point or integral"));
    }
    return ret;
}

} // namespace amt::impl

#endif // AMT_DATAFRAME_FUNCTIONS_BOX_MATHS_HPP
