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
        auto fn = [](auto &&val) { return is_none(val) || is_nan(val); };
        if (holds_type<float>(in)) {
            mean<float>(in, out, std::move(fn));
        } else {
            mean<double>(in, out, std::move(fn));
        }
        return out;
    }

    template <Series SeriesIn, Series SeriesOut, typename MissingType>
    requires(std::is_arithmetic_v<MissingType>) constexpr SeriesOut &mean(
        SeriesIn const &in, SeriesOut &out,
        MissingType const &missing_value) const {
        return mean<MissingType>(in, out, [&missing_value, this](auto &&val) {
            return this->comp(get<MissingType>(val), missing_value);
        });
    }

    template <typename MissingType, Series SeriesIn, Series SeriesOut,
              typename Fn>
    requires(std::is_arithmetic_v<MissingType>) constexpr SeriesOut &mean(
        SeriesIn const &in, SeriesOut &out, Fn &&fn) const {
        if (in.empty())
            return out;

        auto mean_transform = [&, fn = std::forward<Fn>(fn)]() {
            std::size_t to{};
            MissingType s{};

            for (auto const &el : in) {
                if (!std::invoke(fn, el)) {
                    s += get<MissingType>(el);
                    ++to;
                }
            }

            detail::set_dtype(out, dtype<MissingType>());
            auto m = s / static_cast<MissingType>(to);

            transform(in, out, [m, &fn]<typename U>(U const &val) {
                if (std::invoke(fn, val))
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
        auto fn = [](auto &&val) { return is_none(val) || is_nan(val); };
        return freq(in, out, std::move(fn));
    }

    template <Series SeriesIn, Series SeriesOut, typename MissingType>
    constexpr SeriesOut &freq(SeriesIn const &in, SeriesOut &out,
                              MissingType const &missing_value) const {
        auto fn = [&missing_value, this](auto &&val) {
            return this->comp(get<MissingType>(val), missing_value);
        };
        return freq(in, out, std::move(fn));
    }

    template <Series SeriesIn, Series SeriesOut, typename Fn>
    requires(std::is_invocable_v<
             Fn, typename SeriesIn::box_type>) constexpr SeriesOut
        &freq(SeriesIn const &in, SeriesOut &out, Fn &&fn) const {
        if (in.empty())
            return out;

        auto f = ::amt::freq(in);
        auto m = in[0];
        std::size_t co{};

        for (auto &[k, v] : f) {
            if (std::invoke(fn, k))
                continue;
            if (v < co) {
                co = v;
                m = std::move(k);
            }
        }
        if (std::invoke(fn, m)) {
            throw std::runtime_error(
                ERR_CSTR("amt::fn_imputer_t::freq(SeriesIn const&, SeriesOut&, "
                         "Missing const&) : every entry in the series is "
                         "missing_value"));
        }

        transform(in, out,
                  [m = std::move(m), fn = std::forward<Fn>(fn)](auto &&val) {
                      if (std::invoke(fn, val))
                          return m;
                      return val;
                  });
        return out;
    }

    template <Series SeriesIn, Series SeriesOut, typename MissingType>
    constexpr SeriesOut &constant(SeriesIn const &in, SeriesOut &out,
                                  MissingType const &missing_value,
                                  auto &&ct) const {

        auto fn = [&missing_value, this](auto &&val) {
            return this->comp(get<MissingType>(val), missing_value);
        };
        return constant<MissingType>(in, out, std::move(fn),
                                     std::forward<decltype(ct)>(ct));
    }

    template <typename MissingType, Series SeriesIn, Series SeriesOut,
              typename Fn>
    constexpr SeriesOut &constant(SeriesIn const &in, SeriesOut &out, Fn &&fn,
                                  auto &&ct) const {
        if (in.empty())
            return out;

        transform(in, out,
                  [ct = std::move(ct),
                   fn = std::forward<Fn>(fn)]<typename T>(T const &val) {
                      if (std::invoke(fn, val))
                          return T(static_cast<MissingType>(ct),
                                   dtype<MissingType>());
                      return val;
                  });
        return out;
    }

    template <Series SeriesIn, Series SeriesOut>
    constexpr SeriesOut &constant(SeriesIn const &in, SeriesOut &out,
                                  tag::nan_t, auto &&ct) const {
        auto fn = [](auto &&val) { return is_nan(val) || is_none(val); };
        return constant<std::decay_t<decltype(ct)>>(
            in, out, std::move(fn), std::forward<decltype(ct)>(ct));
    }

    template <Series SeriesIn>
    constexpr series_result_t<SeriesIn> mean(SeriesIn const &in) const {
        series_result_t<SeriesIn> out(in.name(), in.size());
        if (is_floating_point(in)) {
            mean(in, out, tag::nan_t{});
        } else {
            out = in;
        }
        return out;
    }

    template <Series SeriesIn>
    constexpr series_result_t<SeriesIn> mean(SeriesIn const &in,
                                             auto &&val) const {
        series_result_t<SeriesIn> out(in.name(), in.size());
        mean(in, out, std::forward<decltype(val)>(val));
        return out;
    }

    template <typename MissingType, Series SeriesIn>
    constexpr series_result_t<SeriesIn> mean(SeriesIn const &in,
                                             auto &&val) const {
        series_result_t<SeriesIn> out(in.name(), in.size());
        mean<MissingType>(in, out, std::forward<decltype(val)>(val));
        return out;
    }

    template <Series SeriesIn>
    constexpr SeriesIn &mean(SeriesIn &in, tag::inplace_t, auto &&val) const {
        mean(in, in, std::forward<decltype(val)>(val));
        return in;
    }

    template <typename MissingType, Series SeriesIn>
    constexpr SeriesIn &mean(SeriesIn &in, tag::inplace_t, auto &&val) const {
        mean<MissingType>(in, in, std::forward<decltype(val)>(val));
        return in;
    }

    template <Series SeriesIn>
    constexpr series_result_t<SeriesIn> freq(SeriesIn const &in,
                                             auto &&val) const {
        series_result_t<SeriesIn> out(in.name(), in.size());
        freq(in, out, std::forward<decltype(val)>(val));
        return out;
    }

    template <Series SeriesIn>
    constexpr SeriesIn &freq(SeriesIn &in, tag::inplace_t, auto &&val) const {
        freq(in, in, std::forward<decltype(val)>(val));
        return in;
    }

    template <Series SeriesIn>
    constexpr series_result_t<SeriesIn> constant(SeriesIn const &in, auto &&val,
                                                 auto &&new_val) const {
        series_result_t<SeriesIn> out(in.name(), in.size());
        constant(in, out, std::forward<decltype(val)>(val),
                 std::forward<decltype(new_val)>(new_val));
        return out;
    }

    template <Series SeriesIn>
    constexpr SeriesIn &constant(SeriesIn &in, tag::inplace_t, auto &&val,
                                 auto &&new_val) const {
        constant(in, in, std::forward<decltype(val)>(val),
                 std::forward<decltype(new_val)>(new_val));
        return in;
    }

    template <typename MissingType, Series SeriesIn>
    constexpr series_result_t<SeriesIn> constant(SeriesIn const &in, auto &&val,
                                                 auto &&new_val) const {
        series_result_t<SeriesIn> out(in.name(), in.size());
        constant<MissingType>(in, out, std::forward<decltype(val)>(val),
                              std::forward<decltype(new_val)>(new_val));
        return out;
    }

    template <typename MissingType, Series SeriesIn>
    constexpr SeriesIn &constant(SeriesIn &in, tag::inplace_t, auto &&val,
                                 auto &&new_val) const {
        constant<MissingType>(in, in, std::forward<decltype(val)>(val),
                              std::forward<decltype(new_val)>(new_val));
        return in;
    }

  private:
    template <typename T> constexpr bool comp(T const &a, T const &b) const {
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
