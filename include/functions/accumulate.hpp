#if !defined(AMT_FUNCTIONS_ACCUMULATE_HPP)
#define AMT_FUNCTIONS_ACCUMULATE_HPP

#include <algorithm>
#include <core/visit.hpp>
#include <operator.hpp>

namespace amt::fn {

struct accumulate_fn {

    template <typename AccumType, typename Fn>
    inline constexpr AccumType operator()(Box auto &&in, AccumType init,
                                          Fn &&fn) const noexcept {

        using storage_type = std::decay_t<decltype(in)>;
        using type_list = typename storage_type::stored_types;

        visit(in, [&, fn = std::move(fn)](auto &&val) {
            using type = std::decay_t<decltype(val)>;

            if constexpr (core::is_generic_binary_at_pos_1_v<AccumType,
                                                             type_list, Fn>) {
                init = std::move(fn(init, val));
            } else {
                using traits = core::function_traits<Fn>;
                using param_type =
                    std::decay_t<typename traits::template arg<1>::type>;
                using nparam_type = core::norm_type_t<param_type>;
                if constexpr (std::is_same_v<type, nparam_type>) {
                    init = std::move(fn(init, val));
                }
            }
        });
        return init;
    }

    template <SeriesViewOrSeries S, typename AccumType, typename Fn>
    inline AccumType operator()(S const &in, AccumType init,
                                Fn &&fn) const noexcept {
        auto sz = in.size();
#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < sz; ++i) {
            decltype(auto) in_el = in[i];
            init = this->operator()(in_el, init, std::move(fn));
        }
        return init;
    }

    template <FrameViewOrFrame F, typename AccumType, typename Fn>
    inline AccumType operator()(F const &in, AccumType init,
                                Fn &&fn) const noexcept {
        auto cols = in.cols();
        auto rows = in.rows();
#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < cols; ++i) {
            for (auto j = 0ul; j < rows; ++j) {
                decltype(auto) in_el = in[i][j];
                init = this->operator()(in_el, init, std::move(fn));
            }
        }
        return init;
    }
};

} // namespace amt::fn

#endif // AMT_FUNCTIONS_ACCUMULATE_HPP
