#if !defined(AMT_DATAFRAME_FUNCTIONS_REPLACE_HPP)
#define AMT_DATAFRAME_FUNCTIONS_REPLACE_HPP

#include <dataframe/box.hpp>
#include <dataframe/functions/transform.hpp>
#include <unordered_map>

namespace amt {

namespace fn {

struct replace_t {
    template <Series SeriesType, Series SeriesOut,
              typename Mapper = std::unordered_map<
                  typename SeriesType::box_type, typename SeriesType::box_type>>
    constexpr SeriesOut &operator()(SeriesType const &in, SeriesOut &out,
                                    Mapper m) const {
        transform(in, out, [m = std::move(m)](auto &&val) {
            if (auto it = m.find(val); it != m.end()) {
                return it->second;
            } else {
                return val;
            }
        });
    }

    template <Series SeriesType,
              typename Mapper = std::unordered_map<
                  typename SeriesType::box_type, typename SeriesType::box_type>>
    constexpr auto operator()(SeriesType const &in, Mapper m) const {
        return transform(in, [m = std::move(m)](auto &&val) {
            if (auto it = m.find(val); it != m.end()) {
                return it->second;
            } else {
                return val;
            }
        });
    }

    template <Series SeriesType,
              typename Mapper = std::unordered_map<
                  typename SeriesType::box_type, typename SeriesType::box_type>>
    constexpr auto &operator()(SeriesType &in, tag::inplace_t,
                               Mapper m) const {
        return transform(in, tag::inplace_t{}, [m = std::move(m)](auto &&val) {
            if (auto it = m.find(val); it != m.end()) {
                return it->second;
            } else {
                return val;
            }
        });
    }
};

} // namespace fn

inline static constexpr auto replace = fn::replace_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_REPLACE_HPP
