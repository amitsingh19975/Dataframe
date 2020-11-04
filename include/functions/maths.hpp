#if !defined(AMT_DATAFRAME_FUNCTIONS_SERIES_MATHS_HPP)
#define AMT_DATAFRAME_FUNCTIONS_SERIES_MATHS_HPP

#include <functions/box_maths.hpp>
#include <functions/transform.hpp>

namespace amt {

namespace fn {

struct abs_t {
    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::abs(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct exp_t {
    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::exp(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct exp2_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::exp2(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct log_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::log(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct log2_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::log2(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct log10_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::log10(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct sqrt_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::sqrt(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct cbrt_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::cbrt(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct sin_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::sin(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct cos_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::cos(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct tan_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::tan(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct asin_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::asin(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct acos_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::acos(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct atan_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::atan(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct sinh_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::sinh(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct cosh_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::cosh(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct tanh_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::tanh(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct asinh_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::asinh(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct acosh_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::acosh(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct atanh_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::atanh(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct ceil_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::ceil(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct floor_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::floor(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct round_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::round(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct trunc_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::trunc(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct isfinite_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::isfinite(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct isinf_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::isinf(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct isnan_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::isnan(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct isnormal_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::isnormal(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct signbit_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &in) const {
        return impl::signbit(in);
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType operator()(DataFrameType const &in) const {
        return transform(
            in, [this](auto const &val) { return this->operator()(val); });
    }

    template <typename DataFrameType>
    requires(Series<DataFrameType> || Frame<DataFrameType>)
    constexpr DataFrameType &operator()(DataFrameType &in, tag::inplace_t) const {
        return transform(in, tag::inplace_t{}, [this](auto const &val) {
            return this->operator()(val);
        });
    }
};

struct pow_t {

    template <typename T, typename U>
    constexpr box_result_t<T, U> operator()(T const &x, U const &y) const {
        return impl::pow(x, y);
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_series_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_series_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.size() != y.size()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::pow_t::operator()(T const&, U const&) : "
                             "size mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_series_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_frame_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_frame_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.shape() != y.shape()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::pow_t::operator()(T const&, U const&) : "
                             "shape mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_frame_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }
};

struct isgreater_t {

    template <typename T, typename U>
    constexpr box_result_t<T, U> operator()(T const &x, U const &y) const {
        return impl::isgreater(x, y);
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_series_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_series_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.size() != y.size()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::isgreater_t::operator()(T const&, U "
                             "const&) : size mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_series_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_frame_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_frame_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.shape() != y.shape()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::isgreater_t::operator()(T const&, U const&) : "
                             "shape mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_frame_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }
};

struct isgreaterequal_t {

    template <typename T, typename U>
    constexpr box_result_t<T, U> operator()(T const &x, U const &y) const {
        return impl::isgreaterequal(x, y);
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_series_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_series_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.size() != y.size()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::isgreaterequal_t::operator()(T const&, "
                             "U const&) : size mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_series_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_frame_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_frame_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.shape() != y.shape()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::isgreaterequal_t::operator()(T const&, U const&) : "
                             "shape mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_frame_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }
};

struct isless_t {

    template <typename T, typename U>
    constexpr box_result_t<T, U> operator()(T const &x, U const &y) const {
        return impl::isless(x, y);
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_series_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_series_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.size() != y.size()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::isless_t::operator()(T const&, U "
                             "const&) : size mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_series_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_frame_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_frame_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.shape() != y.shape()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::isless_t::operator()(T const&, U const&) : "
                             "shape mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_frame_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }
};

struct islessequal_t {

    template <typename T, typename U>
    constexpr box_result_t<T, U> operator()(T const &x, U const &y) const {
        return impl::islessequal(x, y);
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_series_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_series_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.size() != y.size()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::islessequal_t::operator()(T const&, U "
                             "const&) : size mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_series_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_frame_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_frame_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.shape() != y.shape()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::islessequal_t::operator()(T const&, U const&) : "
                             "shape mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_frame_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }
};

struct islessgreater_t {

    template <typename T, typename U>
    constexpr box_result_t<T, U> operator()(T const &x, U const &y) const {
        return impl::islessgreater(x, y);
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_series_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_series_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.size() != y.size()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::islessgreater_t::operator()(T const&, U "
                             "const&) : size mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Series<T> || Series<U>) constexpr series_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_series_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U>
    operator()(T const &x, U const &y) const {
        if constexpr (!is_frame_v<T>) {
            return transform(y, [&x, this](auto const &val) {
                return this->operator()(x, val);
            });
        } else if constexpr (!is_frame_v<U>) {
            return transform(x, [&y, this](auto const &val) {
                return this->operator()(val, y);
            });
        } else {
            if (x.shape() != y.shape()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::fn::islessgreater_t::operator()(T const&, U const&) : "
                             "shape mismatch"));
            }
            return transform(x, y, [this](auto const &xval, auto const &yval) {
                return this->operator()(xval, yval);
            });
        }
    }

    template <typename T, typename U>
    requires(Frame<T> || Frame<U>) constexpr frame_result_t<T, U> &
    operator()(T &&x, U &&y, tag::inplace_t) const {
        auto temp = this->operator()(x, y);
        if constexpr (is_frame_v<T>) {
            x = std::move(temp);
            return x;
        } else {
            y = std::move(temp);
            return y;
        }
    }
};

} // namespace fn

inline static constexpr auto abs = fn::abs_t{};
inline static constexpr auto exp = fn::exp_t{};
inline static constexpr auto exp2 = fn::exp2_t{};
inline static constexpr auto log = fn::log_t{};
inline static constexpr auto log2 = fn::log2_t{};
inline static constexpr auto log10 = fn::log10_t{};
inline static constexpr auto sqrt = fn::sqrt_t{};
inline static constexpr auto cbrt = fn::cbrt_t{};
inline static constexpr auto sin = fn::sin_t{};
inline static constexpr auto cos = fn::cos_t{};
inline static constexpr auto tan = fn::tan_t{};
inline static constexpr auto asin = fn::asin_t{};
inline static constexpr auto acos = fn::acos_t{};
inline static constexpr auto atan = fn::atan_t{};
inline static constexpr auto sinh = fn::sinh_t{};
inline static constexpr auto cosh = fn::cosh_t{};
inline static constexpr auto tanh = fn::tanh_t{};
inline static constexpr auto asinh = fn::asinh_t{};
inline static constexpr auto acosh = fn::acosh_t{};
inline static constexpr auto atanh = fn::atanh_t{};
inline static constexpr auto ceil = fn::ceil_t{};
inline static constexpr auto floor = fn::floor_t{};
inline static constexpr auto round = fn::round_t{};
inline static constexpr auto trunc = fn::trunc_t{};
inline static constexpr auto isfinite = fn::isfinite_t{};
inline static constexpr auto isnan = fn::isnan_t{};
inline static constexpr auto isnormal = fn::isnormal_t{};
inline static constexpr auto signbit = fn::signbit_t{};
inline static constexpr auto pow = fn::pow_t{};
inline static constexpr auto isgreater = fn::isgreater_t{};
inline static constexpr auto isgreaterequal = fn::isgreaterequal_t{};
inline static constexpr auto isless = fn::isless_t{};
inline static constexpr auto islessequal = fn::islessequal_t{};
inline static constexpr auto islessgreater = fn::islessgreater_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_SERIES_MATHS_HPP
