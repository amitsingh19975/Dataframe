#if !defined(AMT_DATAFRAME_FUNCTIONS_STATS_HPP)
#define AMT_DATAFRAME_FUNCTIONS_STATS_HPP

#include <algorithm>
#include <functions/cast.hpp>
#include <functions/reduce.hpp>
#include <functions/transform.hpp>

namespace amt {

namespace fn {

template <typename T = double> struct min_t {
    constexpr T operator()(Series auto &&s) const {
        return *std::min_element(s.begin(), s.end());
    }
};

template <typename T = double> struct max_t {
    constexpr T operator()(Series auto &&s) const {
        return *std::max_element(s.begin(), s.end());
    }
};

template <typename T = double> struct minmax_t {
    constexpr std::pair<T,T> operator()(Series auto &&s) const {
        auto [min,max] = std::minmax_element(s.begin(), s.end());
        return {static_cast<T>(*min), static_cast<T>(*max)};
    }
};

template <typename T = double> struct mean_t {
    constexpr T operator()(Series auto &&s) const {
        T sum = reduce<tag::col_t>(s, T{}, std::plus<T>{});
        return sum / static_cast<T>(s.size());
    }
};

template <typename T = double> struct median_t {
    constexpr T operator()(Series auto &&s) const {
        auto n = s.size() / 2;
        std::nth_element(s.begin(), s.begin() + static_cast<std::ptrdiff_t>(n),
                         s.end());
        return s[n];
    }
};

template <typename T = double> struct var_t {
    constexpr T operator()(Series auto &&s) const {
        T sum{};
        T sq_sum = reduce<tag::col_t>(s, T{}, [&sum](T const &acc, T const &val) {
            sum += val;
            return acc + (val * val);
        });
        auto sz = static_cast<T>(s.size());
        auto mn = sum / sz;
        return (sq_sum / sz) - (mn * mn);
    }

    constexpr T operator()(Series auto &&s, tag::sample_t) const {
        T sum{};
        T sq_sum = reduce<tag::col_t>(s, T{}, [&sum](T const &acc, T const &val) {
            sum += val;
            return acc + (val * val);
        });
        auto sz = static_cast<T>(s.size());
        auto nu = sum / sz;
        sq_sum /= (sz - T(1));
        auto t1 = (T(2) * nu * sum) / (sz - T(1));
        auto t2 = (sz * nu * nu) / (sz - T(1));

        return sq_sum - t1 + t2;
    }
};

template <typename T = double> struct sdev_t {
    constexpr T operator()(Series auto &&s) const {
        return std::sqrt(var_t<T>{}(s));
    }

    constexpr T operator()(Series auto &&s, tag::sample_t) const {
        return std::sqrt(var_t<T>{}(s, tag::sample_t{}));
    }
};

template <typename T = double> struct skewness_t {
    constexpr T operator()(Series auto &&s) const {
        auto d = std::pow(sdev_t<T>{}(s), 3);
        auto mn = mean_t<T>{}(s);

        T cub_sum = reduce<tag::col_t>(s, T{}, [mn](T const &acc, T const &val) {
            auto temp = val - mn;
            return acc + (temp * temp * temp);
        });
        auto sz = static_cast<T>(s.size());
        cub_sum /= d;
        return cub_sum / sz;
    }

    constexpr T operator()(Series auto &&s, tag::sample_t) const {
        auto d = std::pow(sdev_t<T>{}(s, tag::sample_t{}), 3);
        auto mn = mean_t<T>{}(s);

        T cub_sum = reduce<tag::col_t>(s, T{}, [mn](T const &acc, T const &val) {
            auto temp = val - mn;
            return acc + (temp * temp * temp);
        });
        auto sz = static_cast<T>(s.size());
        cub_sum /= d;
        return cub_sum / sz;
    }
};

template <typename T = double> struct kurtosis_t {
    constexpr T operator()(Series auto &&s) const {
        auto d = var_t<T>{}(s);
        d *= d;
        auto mn = mean_t<T>{}(s);

        T cub_sum = reduce<tag::col_t>(s, T{}, [mn](T const &acc, T const &val) {
            auto temp = val - mn;
            return acc + (temp * temp * temp * temp);
        });
        auto sz = static_cast<T>(s.size());
        cub_sum /= d;
        return (cub_sum / sz) - T(3);
    }

    constexpr T operator()(Series auto &&s, tag::sample_t) const {
        auto d = var_t<T>{}(s, tag::sample_t{});
        d *= d;
        auto mn = mean_t<T>{}(s);

        T cub_sum = reduce<tag::col_t>(s, T{}, [mn](T const &acc, T const &val) {
            auto temp = val - mn;
            return acc + (temp * temp * temp * temp);
        });
        auto sz = static_cast<T>(s.size());
        cub_sum /= d;
        return (cub_sum / sz) - T(3);
    }
};

template <typename T = double> struct covar_t {
    constexpr T operator()(Series auto &&x, Series auto &&y) const {
        auto xmn = mean_t<T>{}(x);
        auto ymn = mean_t<T>{}(y);
        auto c = (x - xmn) * (y - ymn);
        auto sz = static_cast<T>(x.size());
        return reduce<tag::col_t>(c, T{}, std::plus<T>{}) / sz;
    }

    constexpr T operator()(Series auto &&x, Series auto &&y, tag::sample_t) const {
        auto xmn = mean_t<T>{}(x);
        auto ymn = mean_t<T>{}(y);
        auto c = (x - xmn) * (y - ymn);
        auto sz = static_cast<T>(x.size()) - T(1);
        return reduce<tag::col_t>(c, T{}, std::plus<T>{}) / sz;
    }
};

template <typename T = double> struct corr_t {
    constexpr T operator()(Series auto &&x, Series auto &&y, tag::sample_t = {}) const {
        auto xdev = sdev_t<T>{}(x);
        auto ydev = sdev_t<T>{}(y);
        auto cov = covar_t<T>{}(x, y);
        return cov / (xdev * ydev);
    }
};

template<typename T = double>
struct minmax_norm_t {
    template<Series SeriesIn, Series SeriesOut>
    constexpr SeriesOut& operator()(SeriesIn const& in, SeriesOut& out) const {
        auto [min,max] = minmax_t<T>{}(in);
        auto diff = max - min;
        transform(in, out, [diff, min = min](auto && val){
            return (val - min) / diff;
        });
        return out;
    }
    
    template<Series SeriesIn>
    constexpr SeriesIn operator()(SeriesIn const& in) const {
        SeriesIn temp(in.name(), in.size());
        this->operator()(in,temp);
        return temp;
    }
    
    template<Series SeriesIn>
    constexpr SeriesIn& operator()(SeriesIn& in, tag::inplace_t) const {
        this->operator()(in,in);
        return in;
    }
};

} // namespace fn

template<typename T = double>
inline static constexpr auto min = fn::min_t<T>{};

template<typename T = double>
inline static constexpr auto max = fn::max_t<T>{};

template<typename T = double>
inline static constexpr auto minmax = fn::minmax_t<T>{};

template<typename T = double>
inline static constexpr auto mean = fn::mean_t<T>{};

template<typename T = double>
inline static constexpr auto median = fn::median_t<T>{};

template<typename T = double>
inline static constexpr auto sdev = fn::sdev_t<T>{};

template<typename T = double>
inline static constexpr auto skewness = fn::skewness_t<T>{};

template<typename T = double>
inline static constexpr auto kurtosis = fn::kurtosis_t<T>{};

template<typename T = double>
inline static constexpr auto covar = fn::covar_t<T>{};

template<typename T = double>
inline static constexpr auto corr = fn::corr_t<T>{};

template<typename T = double>
inline static constexpr auto minmax_norm = fn::minmax_norm_t<T>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_STATS_HPP
