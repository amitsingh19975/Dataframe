#if !defined(AMT_DATAFRAME_FUNCTIONS_CAST_HPP)
#define AMT_DATAFRAME_FUNCTIONS_CAST_HPP

#include <dataframe/frame_utils.hpp>
#include <dataframe/functions/transform.hpp>
#include <dataframe/series_utils.hpp>
#include <dataframe/core/macro.hpp>
#include <optional>
#include <sstream>

namespace amt {

namespace fn {

template <typename To> struct cast_t {
    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &b) const {
        BoxType res(dtype<To>{});
        visit(b, [&res]<typename T>(T &&val) {
            if constexpr (!is_monostate_v<T>) {
                To temp;
                auto op = conv::cast(temp,std::forward<T>(val));
                if (op)
                    res = std::move(temp);
                else {
                    if constexpr (std::numeric_limits<To>::has_quiet_NaN) {
                        res = std::nan("1");
                    } else {
                        throw std::runtime_error(
                            ERR_CSTR("amt::cast_t::operator()(BoxType const &) "
                                     ": bad cast"));
                    }
                }
            } else {
                res = To{};
            }
        });
        return res;
    }

    template <Series SeriesType, Series OutSeriesType>
    constexpr OutSeriesType &operator()(SeriesType const &in,
                                        OutSeriesType &out) const {
        transform(in, out,
                  [this](auto &&val) { return this->operator()(val); });
        if constexpr (PureSeries<OutSeriesType>) {
            detail::set_dtype(out, dtype<To>());
        }
        return out;
    }

    template <Series SeriesType>
    constexpr series_result_t<SeriesType>
    operator()(SeriesType const &in) const {
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
    constexpr OutFrameType &operator()(FrameType const &in,
                                       OutFrameType &out) const {
        for (auto i = 0u; i < in.cols(); ++i) {
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

template <typename To> inline static constexpr auto cast = fn::cast_t<To>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_CAST_HPP
