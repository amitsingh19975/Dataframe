#if !defined(AMT_DATAFRAME_FUNCTIONS_IMPUTER_HPP)
#define AMT_DATAFRAME_FUNCTIONS_IMPUTER_HPP

#include <dataframe/core/macro.hpp>
#include <dataframe/frame_utils.hpp>
#include <dataframe/functions/frequency.hpp>
#include <dataframe/functions/reduce.hpp>
#include <dataframe/functions/transform.hpp>
#include <dataframe/series_utils.hpp>

namespace amt {

namespace fn {

struct imputer_t {

    template <Series SeriesIn, Series SeriesOut>
    constexpr SeriesOut &mean(SeriesIn const &in, SeriesOut &out,
                              tag::nan_t) const {
        if (in.empty())
            return out;

        if (!is_floating_point(in)) {
            throw std::runtime_error(ERR_CSTR(
                "amt::fn::imputer_t::mean(SeriesIn const& in, SeriesOut& out, "
                "tag::nan_t) : series should be floating point"));
        }

        auto mean_transform = [&]<typename T>(T s) {
            std::size_t to{};
            s = reduce_col(in, s, [&to](T acc, T val) {
                if (!std::isnan(val)) {
                    ++to;
                    return acc + val;
                }
                return acc;
            });
            detail::set_dtype(out, dtype<T>());
            auto m = s / static_cast<T>(to);

            transform(in, out, [m]<typename U>(U const &val) {
                if (is_nan(val) || is_none(val))
                    return U(static_cast<T>(m), dtype<T>());
                return val;
            });
        };

        if (holds_type<float>(in)) {
            mean_transform(float{});
        } else {
            mean_transform(double{});
        }
        return out;
    }

    template <Series SeriesIn, Series SeriesOut, typename MissingType>
    requires(std::is_arithmetic_v<MissingType>) constexpr SeriesOut &mean(
        SeriesIn const &in, SeriesOut &out,
        MissingType const &missing_value) const {
        if (in.empty())
            return out;

        auto mean_transform = [&, this]() {
            std::size_t to{};
            MissingType s{};
            s = reduce_col(
                in, s,
                [&to, &missing_value, this](MissingType const &acc, MissingType val) {
                    if (!this->comp(missing_value, val)) {
                        ++to;
                        return static_cast<MissingType>(acc + val);
                    }
                    return acc;
                });
            detail::set_dtype(out, dtype<MissingType>());
            auto m = s / static_cast<MissingType>(to);

            transform(in, out, [m, &missing_value, this]<typename U>(U const &val) {
                if (this->comp(missing_value, get<MissingType>(val)))
                    return U(static_cast<MissingType>(m), dtype<MissingType>());
                return val;
            });
        };

        mean_transform();
        return out;
    }

    template <Series SeriesIn, Series SeriesOut>
    constexpr SeriesOut &freq(SeriesIn const &in, SeriesOut &out,
                              tag::nan_t) const {
        if (in.empty())
            return out;

        auto f = ::amt::freq(in);
        auto m = in[0];
        std::size_t co{};

        for (auto &[k, v] : f) {
            if (is_nan(k) || is_none(k))
                continue;
            if (v < co) {
                co = v;
                m = std::move(k);
            }
        }
        if (is_nan(m) || is_none(m)) {
            throw std::runtime_error(ERR_CSTR(
                "amt::fn_imputer_t::freq(SeriesIn const&, SeriesOut&, "
                "tag::nan_t) : every entry in the series is nan or none"));
        }

        transform(in, out, [m = std::move(m)](auto &&val) {
            if (is_nan(val) || is_none(val))
                return m;
            return val;
        });
        return out;
    }

    template <Series SeriesIn, Series SeriesOut, typename MissingType>
    constexpr SeriesOut &freq(SeriesIn const &in, SeriesOut &out,
                              MissingType const &missing_value) const {
        if (in.empty())
            return out;

        auto f = ::amt::freq(in);
        auto m = in[0];
        std::size_t co{};

        for (auto &[k, v] : f) {
            if (comp(get<MissingType>(k), missing_value))
                continue;
            if (v < co) {
                co = v;
                m = std::move(k);
            }
        }
        if (comp(get<MissingType>(m), missing_value)) {
            throw std::runtime_error(ERR_CSTR(
                "amt::fn_imputer_t::freq(SeriesIn const&, SeriesOut&, "
                "Missing const&) : every entry in the series is missing_value"));
        }


        transform(in, out, [m = std::move(m), &missing_value, this](auto &&val) {
            if (this->comp(get<MissingType>(val), missing_value))
                return m;
            return val;
        });
        return out;
    }

    template <Series SeriesIn, Series SeriesOut, typename MissingType>
    constexpr SeriesOut &constant(SeriesIn const &in, SeriesOut &out,
                                  MissingType const& missing_value, auto &&ct) const {
        if (in.empty())
            return out;

        transform(in, out, [ct = std::move(ct), &missing_value, this]<typename T>(T const &val) {
            if (this->comp(get<MissingType>(val), missing_value))
                return T(static_cast<MissingType>(ct), dtype<MissingType>());
            return val;
        });
        return out;
    }

    template <Series SeriesIn, Series SeriesOut>
    constexpr SeriesOut &constant(SeriesIn const &in, SeriesOut &out,
                                  tag::nan_t, auto &&ct) const {
        if (in.empty())
            return out;

        transform(in, out, [ct = std::move(ct)]<typename T>(T const &val) {
            if (is_nan(val) || is_none(val))
                return T(ct);
            return val;
        });
        return out;
    }

    template <Series SeriesIn>
    constexpr series_result_t<SeriesIn> mean(SeriesIn const &in,
                                             auto&& val) const {
        series_result_t<SeriesIn> out(in.name(), in.size());
        mean(in, out, std::forward<decltype(val)>(val));
        return out;
    }

    template <Series SeriesIn>
    constexpr SeriesIn &mean(SeriesIn &in, tag::inplace_t, auto&& val) const {
        mean(in, in, std::forward<decltype(val)>(val));
        return in;
    }

    template <Series SeriesIn>
    constexpr series_result_t<SeriesIn> freq(SeriesIn const &in,
                                             auto&& val) const {
        series_result_t<SeriesIn> out(in.name(), in.size());
        freq(in, out, std::forward<decltype(val)>(val));
        return out;
    }

    template <Series SeriesIn>
    constexpr SeriesIn &freq(SeriesIn &in, tag::inplace_t, auto&& val) const {
        freq(in, in, std::forward<decltype(val)>(val));
        return in;
    }

    template <Series SeriesIn>
    constexpr series_result_t<SeriesIn> constant(SeriesIn const &in,
                                                 auto&& val, auto&& new_val) const {
        series_result_t<SeriesIn> out(in.name(), in.size());
        constant(in, out, std::forward<decltype(val)>(val), std::forward<decltype(new_val)>(new_val));
        return out;
    }

    template <Series SeriesIn>
    constexpr SeriesIn &constant(SeriesIn &in, tag::inplace_t,
                                 auto&& val, auto&& new_val) const {
        constant(in, in, std::forward<decltype(val)>(val), std::forward<decltype(new_val)>(new_val));
        return in;
    }

  private:
    template<typename T>
    constexpr bool comp(T const &a, T const &b) const {
        if constexpr (std::is_floating_point_v<T>) {
            constexpr auto ep = std::numeric_limits<T>::epsilon();
            return std::abs(a - b) < ep;
        } else {
            return a == b;
        }
    };
};

} // namespace fn

inline static constexpr auto imputer = fn::imputer_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_IMPUTER_HPP
