#if !defined(AMT_DATAFRAME_FUNCTIONS_REDUCE_HPP)
#define AMT_DATAFRAME_FUNCTIONS_REDUCE_HPP

#include <dataframe/box_utils.hpp>
#include <dataframe/core/macro.hpp>
#include <dataframe/slice.hpp>
#include <dataframe/traits/series_traits.hpp>

namespace amt {

namespace fn {

template <typename T> struct reduce_t;

template <> struct reduce_t<tag::col_t> {
    template <typename Accu, typename Fn>
    constexpr Accu operator()(Series auto const &in, Accu acc, Fn &&fn) const {

#pragma omp parallel for schedule(static, 1)
        for (auto it = in.begin(); it != in.end(); ++it) {
            acc = std::invoke(std::forward<Fn>(fn), acc, get<Accu>(*it));
        }

        return acc;
    }

    template <Series Accu, typename Fn>
    constexpr decltype(auto) operator()(Frame auto const &in, Accu &&acc,
                                        Fn &&fn) const {

#pragma omp parallel for schedule(static, 1)
        for (auto i = 0u; i < in.rows(); ++i) {
            acc = std::invoke(std::forward<Fn>(fn), std::forward<Accu>(acc),
                              in.get_row(i));
        }

        return acc;
    }

    template <Frame FrameType, typename Fn>
    constexpr auto operator()(FrameType const &in, Fn &&fn) const {
        using ret_type = typename FrameType::value_type;
        if (in.empty()) {
            return ret_type{};
        }

        ret_type ret(in.get_row(0u));
        this->operator()(in({}, slice(1u)), ret, std::forward<Fn>(fn));
        return ret;
    }
};

template <> struct reduce_t<tag::row_t> {
    template <typename Fn>
    constexpr auto operator()(Series auto const &in, Fn &&) const {
        return in;
    }

    template <typename Fn>
    constexpr auto &operator()(Series auto const &in, Series auto &out,
                               Fn &&) const {
        out = in;
        return out;
    }

    template <typename Fn>
    constexpr decltype(auto) operator()(Frame auto const &in, Series auto &&acc,
                                        Fn &&fn) const {

        for (auto i = 0u; i < in.cols(); ++i) {
            acc = std::invoke(std::forward<Fn>(fn),
                              std::forward<decltype(acc)>(acc), in[i]);
        }

        return acc;
    }

    template <Frame FrameType, typename Fn>
    constexpr auto operator()(FrameType const &in, Fn &&fn) const {
        using ret_type = typename FrameType::value_type;
        if (in.empty()) {
            return ret_type{};
        }

        ret_type ret = in[0];
        ret.name().clear();
        this->operator()(in(slice(1u)), ret, std::forward<Fn>(fn));
        return ret;
    }
};

} // namespace fn

template <typename T> inline static constexpr auto reduce = fn::reduce_t<T>{};

inline static constexpr auto reduce_col = reduce<tag::col_t>;
inline static constexpr auto reduce_row = reduce<tag::row_t>;

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_REDUCE_HPP
