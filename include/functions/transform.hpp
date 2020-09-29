#if !defined(AMT_FUNCTIONS_TRANSFORM_HPP)
#define AMT_FUNCTIONS_TRANSFORM_HPP

#include <algorithm>
#include <core/visit.hpp>
#include <operator.hpp>

namespace amt::fn {

struct transform_fn {

    template <typename Fn>
    inline constexpr decltype(auto) operator()(Box auto &&in, Box auto &out,
                                               Fn &&fn) const noexcept {

        using storage_type = std::decay_t<decltype(in)>;
        using type_list = typename storage_type::stored_types;

        visit(in, [&, fn = std::move(fn)](auto &&val) {
            using type = std::decay_t<decltype(val)>;

            if constexpr (core::is_generic_unary_v<type_list, Fn>) {
                out = std::move(fn(val));
            } else {
                using traits = core::function_traits<Fn>;
                using param_type =
                    std::decay_t<typename traits::template arg<0>::type>;
                using nparam_type = core::norm_type_t<param_type>;
                if constexpr (std::is_same_v<type, nparam_type>) {
                    out = std::move(fn(val));
                }
            }
        });
        return out;
    }

    template <Tag T, typename Fn>
    inline constexpr decltype(auto)
    operator()(Box auto &&s, [[maybe_unused]] T t, Fn &&fn) const noexcept {
        using storage_type = std::decay_t<decltype(s)>;

        auto temp = storage_type{};
        this->operator()(s, temp, std::forward<Fn>(fn));
        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return s;
        } else {
            return temp;
        }
    }

    template <SeriesViewOrSeries S1, SeriesViewOrSeries S2, typename Fn>
    inline decltype(auto) operator()(S1 const &in, S2 &out,
                                     Fn &&fn) const noexcept {
        auto sz = in.size();

        if constexpr (!is_view_v<std::decay_t<S2>>) {
            out.resize(sz);
        }

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < sz; ++i) {
            decltype(auto) in_el = in[i];
            decltype(auto) out_el = out[i];
            this->operator()(in_el, out_el, std::move(fn));
        }
        return static_cast<S2 &>(out);
    }

    template <Tag T, SeriesViewOrSeries S, typename Fn>
    inline constexpr decltype(auto) operator()(S &&s, [[maybe_unused]] T t,
                                               Fn &&fn) const {

        result_type_t<S> temp;
        this->operator()(std::forward<S>(s), temp, std::forward<Fn>(fn));

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<S>(s);
        } else {
            return temp;
        }
    }

    template <FrameViewOrFrame F1, FrameViewOrFrame F2, typename Fn>
    inline decltype(auto) operator()(F1 const &in, F2 &out,
                                     Fn &&fn) const noexcept {
        auto cols = in.cols();
        auto rows = in.rows();
        using value_type = typename std::decay_t<F2>::value_type;

        if constexpr (!is_view_v<std::decay_t<F2>>) {
            out.resize(cols, value_type(rows));
        }

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < cols; ++i) {
            for (auto j = 0ul; j < rows; ++j) {
                decltype(auto) in_el = in[i][j];
                decltype(auto) out_el = out[i][j];
                this->operator()(in_el, out_el, std::move(fn));
            }
        }
        return static_cast<F2 &>(out);
    }

    template <Tag T, FrameViewOrFrame F, typename Fn>
    inline constexpr decltype(auto) operator()(F &&s, [[maybe_unused]] T t,
                                               Fn &&fn) const {

        result_type_t<F> temp;
        this->operator()(std::forward<F>(s), temp, std::forward<Fn>(fn));

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<F>(s);
        } else {
            return temp;
        }
    }
};

} // namespace amt::fn

#endif // AMT_FUNCTIONS_TRANSFORM_HPP
