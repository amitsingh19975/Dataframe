#if !defined(AMT_DATAFRAME_FUNCTIONS_GET_DUMMIES_HPP)
#define AMT_DATAFRAME_FUNCTIONS_GET_DUMMIES_HPP

#include <dataframe/traits/series_traits.hpp>
#include <dataframe/core/utils.hpp>
#include <dataframe/traits/frame_traits.hpp>
#include <dataframe/functions/unqiue.hpp>
#include <dataframe/functions/transform.hpp>
#include <dataframe/functions/cast.hpp>
#include <dataframe/functions/column.hpp>
#include <vector>

namespace amt {

namespace fn {

template<typename GetDummies>
struct get_dummies_col_t : column_t{

    template<typename... Ts>
    constexpr get_dummies_col_t(Ts&&... args) noexcept
        : column_t(std::forward<Ts>(args)...)
    {}

    template<Frame FrameType, PureFrame FrameOut>
    constexpr FrameOut& operator()(FrameType const& in, FrameOut& out, bool drop_first = false) const{
        out.set_dtypes(in);
        std::size_t id{};
        if ( has_index() ){
            id = index();
        }else{
            for(auto const& s : in){
                if( s.name() == name() ) break;
                ++id;
            }
        }

        auto temp_f = m_dummies(in[id],drop_first);
        out.resize(in.cols() - 1ul + temp_f.size());
        
        auto k = 0ul;
        for(auto i = 0ul; i < in.cols(); ++i){
            if(i == id) {
                for(auto j = 0ul; j < temp_f.cols(); ++j){
                    out[k++] = std::move(temp_f[j]);
                }
            }else{
                out[k++] = in[i];
            }
        }

        return out;
    }

    template<Frame FrameType>
    constexpr frame_result_t<FrameType> operator()(FrameType const& in, bool drop_first = false) const{
        frame_result_t<FrameType> out(in.cols());
        this->operator()(in, out, drop_first);
        return out;
    }

    template<PureFrame FrameType>
    constexpr FrameType& operator()(FrameType& in, tag::inplace_t, bool drop_first = false) const{
        auto out = this->operator()(in, drop_first);
        in = std::move(out);
        return in;
    }
private:
    GetDummies m_dummies{};
};

template<typename DataType = double>
struct get_dummies_t {

    template<Series SeriesIn, PureFrame FrameOut, typename BoxType = typename SeriesIn::box_type>
    constexpr FrameOut& operator()(SeriesIn const& in, FrameOut& out, bool drop_first = false) const{
        auto uq = unique(in);
        out.resize(uq.size() - static_cast<std::size_t>(drop_first), in.size(), BoxType{ DataType(0), dtype<DataType>() });
        out.dtype( dtype<DataType>() );

        std::vector<BoxType> v;
        v.reserve(uq.size());

        auto k = 0ul;
        for(auto& el : uq) v.emplace_back(std::move(el));

        k = 0ul;
        for(auto i = static_cast<std::size_t>(drop_first); i < v.size(); ++i, ++k){
            auto const& el = v[i];
            auto str = std::move(get<std::string>(cast<std::string>(el)));
            out.name(k, str);
            
            for(auto j = 0ul; j < in.size(); ++j){
                if( in[j] == el ){
                    out[k][j] = DataType(1);
                }
            }
        }

        return out;
    }
    
    template<Series SeriesIn, typename BoxType = typename SeriesIn::box_type>
    constexpr auto operator()(SeriesIn const& in, bool drop_first = false) const{
        basic_frame<BoxType> out;
        this->operator()(in, out, drop_first);
        return out;
    }

    template<typename C>
    constexpr get_dummies_col_t<get_dummies_t> operator[](C&& key) const noexcept{
        return {std::forward<C>(key)};
    }

};

} // namespace fn

template<typename DataType = double>
inline static constexpr auto get_dummies = fn::get_dummies_t<DataType>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_GET_DUMMIES_HPP
