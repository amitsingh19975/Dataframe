#if !defined(AMT_DATAFRAME_FUNCTIONS_CAST_HPP)
#define AMT_DATAFRAME_FUNCTIONS_CAST_HPP

#include <series_utils.hpp>
#include <frame_utils.hpp>
#include <functions/transform.hpp>
#include <optional>
#include <sstream>

namespace amt {

namespace fn {

    namespace detail{
        template<typename To, typename U>
        std::optional<To> cast_helper(U&& val) {
            static std::stringstream ss;
            ss.str("");
            ss << std::forward<U>(val);
            To temp;
            ss >> temp;
            if(ss.fail()){
                return std::nullopt;
            }else{
                return { std::move(temp) };
            }
        }
    }

template <typename To> struct cast_t {
    template <Box BoxType> constexpr BoxType operator()(BoxType const &b) const {
        BoxType res( dtype<To>{} );
        visit(b, [&res]<typename T>(T &&val) {
            if constexpr (!is_monostate_v<T>) {
                auto op = detail::cast_helper<To>(std::forward<T>(val));
                if(op) res = std::move(*op);
                else{
                    if constexpr (std::numeric_limits<To>::has_quiet_NaN) {
                        res = To(NAN);
                    } else {
                        throw std::runtime_error(
                            ERR_CSTR("amt::cast_t::operator()(BoxType const &) : bad cast")
                        );
                    }
                }
            } else {
                res = To{};
            }
        });
        return res;
    }

    template <Series SeriesType, Series OutSeriesType>
    constexpr OutSeriesType &operator()(SeriesType const &in, OutSeriesType &out) const {
        if constexpr(PureSeries<OutSeriesType>){
            ::amt::detail::set_dtype(out,dtype<To>());
        }
        transform(in, out,
                         [this](auto &&val) { return this->operator()(val); });
        return out;
    }

    template <Series SeriesType>
    constexpr series_result_t<SeriesType> operator()(SeriesType const &in) const {
        using ret_type = series_result_t<SeriesType>;
        ret_type temp(in.size());
        this->operator()(in, temp);
        return temp;
    }

    template <Series SeriesType>
    constexpr SeriesType &operator()(SeriesType &in, tag::inplace_t) const {
        this->operator()(in, in);
        return in;
    }

    template <Frame FrameType, Frame OutFrameType>
    constexpr OutFrameType &operator()(FrameType const &in, OutFrameType &out) const {
        for(auto i = 0u; i < in.cols(); ++i){
            this->operator()(in[i], out[i]);
        }
        return out;
    }

    template <Frame FrameType>
    constexpr frame_result_t<FrameType> operator()(FrameType const &in) const {
        using ret_type = frame_result_t<FrameType>;
        ret_type temp(in.shape());
        this->operator()(in, temp);
        return temp;
    }

    template <Frame FrameType>
    constexpr FrameType &operator()(FrameType &in, tag::inplace_t) const {
        this->operator()(in, in);
        return in;
    }
};

template <typename To> struct try_cast_t {
    template <Box BoxType>
    constexpr std::optional<BoxType> operator()(BoxType const &b) const {
        return visit(b, [this]<typename T>(T const &val) -> std::optional<BoxType> const {
            if constexpr (is_monostate_v<T>) {
                return std::nullopt;
            } else {
                auto op = detail::cast_helper<To>(val);
                if(op){
                    return {std::move(*op)};
                }else{
                    if constexpr (std::numeric_limits<To>::has_quiet_NaN) {
                        return {NAN};
                    }
                    return std::nullopt;
                }
            }
        });
    }

    template <Series SeriesType, Series OutSeriesType>
    constexpr OutSeriesType &operator()(SeriesType const &in, OutSeriesType &out) const {
        return transform(in, out, [this](auto &&val) {
            auto res = this->operator()(val);
            return res.value_or(val);
        });
    }

    template <Series SeriesType>
    constexpr series_result_t<SeriesType> operator()(SeriesType const &in) const {
        using ret_type = series_result_t<SeriesType>;
        ret_type temp(in.size());
        this->operator()(in, temp);
        return temp;
    }

    template <Series SeriesType>
    constexpr SeriesType &operator()(SeriesType &in, tag::inplace_t) const {
        this->operator()(in, in);
        return in;
    }

    template <Frame FrameType, Frame OutFrameType>
    constexpr OutFrameType &operator()(FrameType const &in, OutFrameType &out) const {
        for(auto i = 0u; i < in.cols(); ++i){
            this->operator()(in[i], out[i]);
        }
        return out;
    }

    template <Frame FrameType>
    constexpr frame_result_t<FrameType> operator()(FrameType const &in) const {
        using ret_type = frame_result_t<FrameType>;
        ret_type temp(in.shape());
        this->operator()(in, temp);
        return temp;
    }

    template <Frame FrameType>
    constexpr FrameType &operator()(FrameType &in, tag::inplace_t) const {
        this->operator()(in, in);
        return in;
    }
};

} // namespace fn

template<typename To>
inline static constexpr auto cast = fn::cast_t<To>{};

template<typename To>
inline static constexpr auto try_cast = fn::try_cast_t<To>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_CAST_HPP
