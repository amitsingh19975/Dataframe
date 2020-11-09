#if !defined(AMT_DATAFRAME_FUNCTIONS_CAST_HPP)
#define AMT_DATAFRAME_FUNCTIONS_CAST_HPP

#include <array>
#include <dataframe/core/macro.hpp>
#include <dataframe/core/nan_list.hpp>
#include <dataframe/frame_utils.hpp>
#include <dataframe/functions/transform.hpp>
#include <dataframe/series_utils.hpp>
#include <optional>
#include <sstream>

namespace amt {

namespace fn {

template <typename To, bool ApplyDefaultNaList = true> struct cast_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &b) const {
        return this->operator()(b, {});
    }

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &b,
                                 name_list const &na_list) const {
        BoxType out;
        set_to_none(b, out, na_list);
        if constexpr (ApplyDefaultNaList) {
            set_to_none(b, out, default_nan_list);
        }
        if( is<To>(b) ){
            return out;
        }else{
            return helper(out);
        }
    }

    template <Series SeriesType, Series OutSeriesType>
    constexpr OutSeriesType &operator()(SeriesType const &in,
                                        OutSeriesType &out,
                                        name_list const &na_list = {}) const {
        transform(in, out, [&na_list, this](auto &&val) {
            return this->operator()(val, na_list);
        });
        if constexpr (PureSeries<OutSeriesType>) {
            detail::set_dtype(out, dtype<To>());
        }
        return out;
    }

    template <Series SeriesType>
    constexpr series_result_t<SeriesType>
    operator()(SeriesType const &in, name_list const &na_list = {}) const {
        using ret_type = series_result_t<SeriesType>;
        ret_type temp(in.size());
        this->operator()(in, temp, na_list);
        return temp;
    }

    template <Series SeriesType>
    constexpr SeriesType &operator()(SeriesType &in, tag::inplace_t,
                                     name_list const &na_list = {}) const {
        this->operator()(in, in, na_list);
        return in;
    }

    template <Frame FrameType, Frame OutFrameType>
    constexpr OutFrameType &operator()(FrameType const &in, OutFrameType &out,
                                       name_list const &na_list = {}) const {
        for (auto i = 0u; i < in.cols(); ++i) {
            this->operator()(in[i], out[i], na_list);
        }
        return out;
    }

    template <Frame FrameType>
    constexpr frame_result_t<FrameType>
    operator()(FrameType const &in, name_list const &na_list = {}) const {
        using ret_type = frame_result_t<FrameType>;
        ret_type temp(in.shape());
        this->operator()(in, temp, na_list);
        return temp;
    }

    template <Frame FrameType>
    constexpr FrameType &operator()(FrameType &in, tag::inplace_t,
                                    name_list const &na_list = {}) const {
        this->operator()(in, in, na_list);
        return in;
    }

  private:
    template <Box BoxType> constexpr BoxType helper(BoxType const &b) const {
        BoxType res(dtype<To>{});
        visit(b, [&res]<typename T>(T &&val) {
            if constexpr (!is_monostate_v<T>) {
                To temp;
                auto op = conv::cast(temp, std::forward<T>(val));
                if (op)
                    res = std::move(temp);
                else {
                    if constexpr (std::numeric_limits<To>::has_quiet_NaN) {
                        res = To(NAN);
                    } else {
                        throw std::runtime_error(
                            ERR_CSTR("amt::cast_t::operator()(BoxType const &) "
                                     ": bad cast"));
                    }
                }
            } else {
                if constexpr (std::numeric_limits<To>::has_quiet_NaN) {
                    res = To(NAN);
                }
            }
        });
        return res;
    }

    template <Box BoxType, typename List>
    void set_to_none(BoxType const &in, BoxType &out, List const &li) const {
        out = in;
        if(li.empty()) return;
        using value_type = typename List::value_type;
        
        if constexpr( std::is_constructible_v<std::string, value_type> ){
            if(is<std::string>(in)){
                if( auto it = std::find(li.begin(), li.end(), get<std::string>(in)); it != li.end() ){
                    out = BoxType{};
                }
            }
        }else{
            if(is<value_type>(in)){
                if( auto it = std::find(li.begin(), li.end(), get<value_type>(in)); it != li.end() ){
                    out = BoxType{};
                }
            }
        }
    }
};

} // namespace fn

template <typename To, bool ApplyDefaultNaList = true>
inline static constexpr auto cast = fn::cast_t<To, ApplyDefaultNaList>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_CAST_HPP
