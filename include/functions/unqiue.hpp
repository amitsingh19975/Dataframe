#if !defined(AMT_DATAFRAME_FUNCTIONS_UNIQUE_HPP)
#define AMT_DATAFRAME_FUNCTIONS_UNIQUE_HPP

#include <traits/series_traits.hpp>
#include <unordered_set>

namespace amt {

namespace fn{
    
    struct unique_t{
        template<Series SeriesType>
        auto operator()(SeriesType&& in) const{
            using series_type = series_result_t<SeriesType>;
            using box_type = typename series_type::value_type;
            std::unordered_set<box_type> ret;
            
            for(auto const& el : in){
                ret.insert(el);
            }
            return ret;
        }
    };

} // namespace fn

inline static constexpr auto unique = fn::unique_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_UNIQUE_HPP
