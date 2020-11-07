#if !defined(AMT_DATAFRAME_FUNCTIONS_FREQUENCY_HPP)
#define AMT_DATAFRAME_FUNCTIONS_FREQUENCY_HPP

#include <dataframe/traits/series_traits.hpp>
#include <unordered_map>

namespace amt {

namespace fn {

struct freq_t {
    template <Series SeriesType> auto operator()(SeriesType &&in) const {
        using series_type = series_result_t<SeriesType>;
        using box_type = typename series_type::value_type;
        std::unordered_map<box_type, std::size_t> map;

        for (auto const &el : in) {
            if (auto it = map.find(el); it != map.end()) {
                ++(it->second);
            } else {
                map[el] = 1u;
            }
        }
        return map;
    }
};

} // namespace fn

inline static constexpr auto freq = fn::freq_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_FREQUENCY_HPP
