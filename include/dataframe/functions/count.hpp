#if !defined(AMT_DATAFRAME_FUNCTIONS_COUNT_HPP)
#define AMT_DATAFRAME_FUNCTIONS_COUNT_HPP

#include <dataframe/traits/series_traits.hpp>

namespace amt {

namespace fn {

struct count_t {
    template <Series SeriesType, typename Fn>
    constexpr std::size_t operator()(SeriesType const &in, Fn &&fn) const {
        std::size_t ct{};
        for (auto const &el : in) {
            if (std::invoke(std::forward<Fn>(fn), el))
                ++ct;
        }
        return ct;
    }

    template <Series SeriesType>
    constexpr std::size_t operator()(SeriesType const &in) const {
        return in.size();
    }
};

} // namespace fn

inline static constexpr auto count = fn::count_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_COUNT_HPP
