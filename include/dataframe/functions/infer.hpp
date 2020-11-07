#if !defined(AMT_DATAFRAME_FUNCTIONS_INFER_HPP)
#define AMT_DATAFRAME_FUNCTIONS_INFER_HPP

#include <dataframe/functions/cast.hpp>
#include <dataframe/traits/basic_traits.hpp>

namespace amt {

namespace fn {

struct infer_t {
    
    template<Box BoxType>
    constexpr BoxType operator()(BoxType const& b) const{
        if(box_infer<long long>(b)){
            return cast<long long>(b);
        }else if(box_infer<double>(b)){
            return cast<double>(b);
        }else{
            return cast<std::string>(b);
        }
    }
    
    template<Series SeriesType, Series SeriesOut>
    constexpr SeriesOut& operator()(SeriesType const& in, SeriesOut& out) const{
        std::size_t count[3] = {0};
        auto sz = in.size();
        for(auto i = 0ul; i < sz; ++i){
            auto& b = in[i];
            if(box_infer<long long>(b)){
                ++count[INT64];
                ++count[DOUBLE];
            }else if(box_infer<double>(b)){
                ++count[DOUBLE];
            }else{
                ++count[STRING];
            }
        }
        
        if(sz == count[INT64]){
            cast<long long>(in,out);
        }else if (count[DOUBLE] > count[STRING]){
            cast<double>(in,out);
        }else{
            cast<std::string>(in,out);
        }
        return out;
    }

    template<Series SeriesType>
    constexpr series_result_t<SeriesType> operator()(SeriesType const& in) const{
        series_result_t<SeriesType> out(in.name(), in.size());
        this->operator()(in, out);
        return out;
    }

    template<Series SeriesType>
    constexpr series_result_t<SeriesType> operator()(SeriesType& in, tag::inplace_t) const{
        auto out = this->operator()(in);
        in = std::move(out);
        return in;
    }
    
    template<Frame FrameType, Frame FrameOut>
    constexpr FrameOut& operator()(FrameType const& in, FrameOut& out) const{
        for(auto i = 0u; i < in.cols(); ++i){
            this->operator()(in[i], out[i]);
        }
        return out;
    }
    
    template<Frame FrameType>
    constexpr frame_result_t<FrameType> operator()(FrameType const& in) const{
        frame_result_t<FrameType> out(in.shape());
        this->operator()(in, out);
        return out;
    }
    
    template<Frame FrameType>
    constexpr frame_result_t<FrameType> operator()(FrameType& in, tag::inplace_t) const{
        auto out = this->operator()(in);
        in = std::move(out);
        return in;
    }

private:

    enum : unsigned char{
        INT64 = 0u, DOUBLE = 1u, STRING = 2u
    };
    
    template <typename To, typename U> bool can_be_casted_helper(U &&val) const {
        if constexpr (conv::HasCast<To,U>){
            To temp;
            return conv::cast(temp, std::forward<U>(val));
        }
        return false;
    }

    template<typename To>
    constexpr bool box_infer(Box auto const &b) const {
        return visit(b, [this](auto const& u){
            return this->can_be_casted_helper<To>(u);
        });
    }
};

} // namespace fn

inline static constexpr auto infer = fn::infer_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_INFER_HPP
