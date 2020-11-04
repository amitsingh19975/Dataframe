#if !defined(AMT_DATAFRAME_FUNCTIONS_CONCAT_HPP)
#define AMT_DATAFRAME_FUNCTIONS_CONCAT_HPP

#include <algorithm>
#include <box_utils.hpp>
#include <core/macro.hpp>
#include <traits/frame_traits.hpp>

namespace amt {

namespace fn {

template <typename Axis> struct concat_t;

template <> struct concat_t<tag::row_t> {
    template <Series SeriesTypeIn1, Series SeriesTypeIn2,
              PureSeries SeriesTypeOut>
    constexpr SeriesTypeOut &operator()(SeriesTypeIn1 const &in1,
                                        SeriesTypeIn2 const &in2,
                                        SeriesTypeOut &out) const {
        auto it = std::copy(in1.begin(), in1.end(), out.begin());
        std::copy(in2.begin(), in2.end(), it);
        return out;
    }

    template <Series SeriesTypeIn1, Series SeriesTypeIn2>
    constexpr series_result_t<SeriesTypeIn1>
    operator()(SeriesTypeIn1 const &in1, SeriesTypeIn2 const &in2) const {
        using out_type = series_result_t<SeriesTypeIn1>;
        auto out = out_type(in1.name(), in1.size() + in2.size());
        this->operator()(in1, in2, out);
        return out;
    }

    template <PureSeries SeriesTypeIn1, Series SeriesTypeIn2>
    constexpr SeriesTypeIn1 &operator()(SeriesTypeIn1 &inout,
                                        SeriesTypeIn2 const &in,
                                        tag::inplace_t) const {
        inout.reserve(inout.size() + in.size());
        std::copy(in.begin(), in.end(), std::back_inserter(inout));
        return inout;
    }

    template <Frame FrameTypeIn1, Frame FrameTypeIn2, PureFrame FrameTypeOut>
    constexpr FrameTypeOut &operator()(FrameTypeIn1 const &in1,
                                       FrameTypeIn2 const &in2,
                                       FrameTypeOut &out) const {
        if (in1.cols() != in2.cols()) {
            throw std::runtime_error(ERR_CSTR(
                "amt::concat<amt::tag::row_t>::operator()(FrameTypeIn1 const "
                "&, FrameTypeIn2 const &, FrameTypeOut&) : cols mismatch"));
        }
        out = in1;
        for (auto i = 0u; i < in2.rows(); ++i) {
            out.row_push_back(in2.get_row(i));
        }
        return out;
    }

    template <Frame FrameTypeIn1, Frame FrameTypeIn2>
    constexpr frame_result_t<FrameTypeIn1>
    operator()(FrameTypeIn1 const &in1, FrameTypeIn2 const &in2) const {
        using out_type = frame_result_t<FrameTypeIn1>;
        auto out = out_type(in1.cols(), in1.rows() + in2.rows());
        this->operator()(in1, in2, out);
        return out;
    }

    template <PureFrame FrameTypeIn1, Frame FrameTypeIn2>
    constexpr FrameTypeIn1 &operator()(FrameTypeIn1 &inout,
                                       FrameTypeIn2 const &in,
                                       tag::inplace_t) const {
        auto temp = this->operator()(inout, in);
        inout = std::move(temp);
        return inout;
    }
};

template <> struct concat_t<tag::col_t> {
    template <Series SeriesTypeIn1, Series SeriesTypeIn2,
              PureFrame FrameTypeOut>
    constexpr FrameTypeOut &operator()(SeriesTypeIn1 const &in1,
                                       SeriesTypeIn2 const &in2,
                                       FrameTypeOut &out) const {

        series_result_t<SeriesTypeIn1> temp1 = in1;
        series_result_t<SeriesTypeIn2> temp2 = in2;
        auto sz = std::max(temp1.size(), temp2.size());
        temp1.resize(sz);
        temp2.resize(sz);
        out.col_push_back(std::move(temp1));
        out.col_push_back(std::move(temp2));
        return out;
    }

    template <Series SeriesTypeIn1, Series SeriesTypeIn2>
    constexpr auto operator()(SeriesTypeIn1 const &in1,
                              SeriesTypeIn2 const &in2) const {
        using value_type = typename SeriesTypeIn1::value_type;
        using ret_type = basic_frame<value_type>;
        auto out = ret_type{};
        this->operator()(in1, in2, out);
        return out;
    }

    template <Frame FrameTypeIn1, Frame FrameTypeIn2, PureFrame FrameTypeOut>
    constexpr FrameTypeOut &operator()(FrameTypeIn1 const &in1,
                                      FrameTypeIn2 const &in2,
                                      FrameTypeOut &out) const {
        out = in1;
        frame_result_t<FrameTypeIn2> temp2 = in2;
        auto sz = std::max(out.rows(), in2.rows());
        out.resize(out.cols(), sz);
        temp2.resize(temp2.cols(), sz);
        for (auto &el : temp2)
            out.col_push_back(std::move(el));
        return out;
    }

    template <Frame FrameTypeIn, Series SeriesType, PureFrame FrameTypeOut>
    constexpr FrameTypeOut &operator()(FrameTypeIn const &in1,
                                      SeriesType const &in2,
                                      FrameTypeOut &out) const {
        out = in1;
        series_result_t<SeriesType> temp2 = in2;
        auto sz = std::max(out.rows(), in2.size());
        out.resize(out.cols(), sz);
        temp2.resize(sz);
        out.col_push_back(std::move(temp2));
        return out;
    }

    template <Frame FrameTypeIn, Series SeriesType, PureFrame FrameTypeOut>
    constexpr FrameTypeOut &operator()(SeriesType const &in1,
                                      FrameTypeIn const &in2,
                                      FrameTypeOut &out) const {
        series_result_t<SeriesType> temp1 = in1;
        out = in2;
        auto sz = std::max(out.rows(), temp1.size());
        temp1.resize(sz);
        out.resize(out.cols(), sz);
        out.insert(out.begin(), std::move(temp1), tag::col);
        return out;
    }

    template <Frame FrameTypeIn, typename DataFrame>
    requires(Series<DataFrame> || Frame<DataFrame>)
    constexpr frame_result_t<FrameTypeIn>
    operator()(FrameTypeIn const &in1, DataFrame const &in2) const {
        using out_type = frame_result_t<FrameTypeIn>;
        auto out = out_type{};
        this->operator()(in1, in2, out);
        return out;
    }

    template <Frame FrameTypeIn, Series SeriesType>
    constexpr frame_result_t<FrameTypeIn>
    operator()(SeriesType const &in1, FrameTypeIn const &in2) const {
        using out_type = frame_result_t<FrameTypeIn>;
        auto out = out_type{};
        this->operator()(in1, in2, out);
        return out;
    }

    template <PureFrame FrameTypeIn, typename DataFrame>
    requires(Series<DataFrame> || Frame<DataFrame>)
    constexpr FrameTypeIn &operator()(FrameTypeIn &inout,
                                       DataFrame const &in,
                                       tag::inplace_t) const {
        auto temp = this->operator()(inout, in);
        inout = std::move(temp);
        return inout;
    }
};

} // namespace fn

template <typename T> inline static constexpr auto concat = fn::concat_t<T>{};

inline static constexpr auto concat_row = fn::concat_t<tag::row_t>{};
inline static constexpr auto concat_col = fn::concat_t<tag::col_t>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_CONCAT_HPP
