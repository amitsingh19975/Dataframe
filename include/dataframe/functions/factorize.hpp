#if !defined(AMT_DATAFRAME_FUNCTIONS_FACTORIZE_HPP)
#define AMT_DATAFRAME_FUNCTIONS_FACTORIZE_HPP

#include <dataframe/traits/series_traits.hpp>
#include <dataframe/functions/unqiue.hpp>
#include <dataframe/functions/transform.hpp>
#include <vector>
#include <unordered_map>

namespace amt {

namespace fn {

template<typename DataType = double>
struct factorize_t {
    
    static_assert(std::numeric_limits<DataType>::is_signed);

    template<Series SeriesIn, PureSeries SeriesOut, typename BoxType = typename SeriesIn::box_type>
    constexpr std::pair< std::vector<BoxType>, SeriesOut& > operator()(SeriesIn const& in, SeriesOut& out, DataType na_sentinel = -1) const{
        auto uq = unique(in);
        std::vector<BoxType> cat(uq.begin(), uq.end());
        std::unordered_map<BoxType, std::size_t> mp;
        for(auto i = 0ul; i < cat.size(); ++i) mp[ cat[i] ] = i;
        
        out.name(in.name());

        transform(in, out, [&](auto const& val){
            if (is_none(val) || is_nan(val)){
                return na_sentinel;
            }else{
                return static_cast<DataType>(mp[val]);
            }
        });
        detail::set_dtype(out,dtype<DataType>());
        return { std::move(cat), out };
    }
    
    template<Series SeriesIn, typename BoxType = typename SeriesIn::box_type>
    constexpr std::pair< std::vector<BoxType>, series_result_t<SeriesIn> > operator()(SeriesIn const& in, long long na_sentinel = -1) const{
        series_result_t<SeriesIn> out(in.name(), in.size(), dtype<long long>());
        auto [cat,_] = this->operator()(in, out, na_sentinel);
        return { std::move(cat), std::move(out) };
    }

    template<PureSeries SeriesIn, typename BoxType = typename SeriesIn::box_type>
    constexpr std::pair< std::vector<BoxType>, SeriesIn& > operator()(SeriesIn& in, tag::inplace_t, long long na_sentinel = -1) const{
        auto [cat, out] = this->operator()(in, na_sentinel);
        in = std::move(out);
        return { std::move(cat), in };
    }

};

} // namespace fn

template<typename DataType = double>
inline static constexpr auto factorize = fn::factorize_t<DataType>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_FACTORIZE_HPP
