#if !defined(AMT_DATAFRAME_FUNCTIONS_TO_VECTOR_HPP)
#define AMT_DATAFRAME_FUNCTIONS_TO_VECTOR_HPP

#include <dataframe/series_utils.hpp>
#include <vector>

namespace amt {

namespace fn {

template <typename DataType> struct to_vector_t {
    template <Series SeriesType>
    constexpr std::vector<DataType> &
    operator()(SeriesType const &in, std::vector<DataType> &out) const {
        if (!is<DataType>(in)) {
            throw std::runtime_error(
                ERR_CSTR("amt::to_vector_t::operator()(SeriesType const&, "
                         "std::vector<DataType>&) : series does not hold dtype "
                         "DataType"));
        }
        for(auto i = 0ul; i < in.size(); ++i){
            auto const& el = in[i];
            if( is_none(el) ){
                out[i] = DataType{};
            }else{
                out[i] = el;
            }
        }
        return out;
    }

    template <Series SeriesType>
    std::vector<DataType> operator()(SeriesType const &in) const {
        std::vector<DataType> ret(in.size());
        this->operator()(in, ret);
        return ret;
    }
};

} // namespace fn

template<typename DataType>
inline static constexpr auto to_vector = fn::to_vector_t<DataType>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_TO_VECTOR_HPP
