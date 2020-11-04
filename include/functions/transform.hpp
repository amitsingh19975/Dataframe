#if !defined(AMT_DATAFRAME_FUNCTIONS_TRANSFORM_HPP)
#define AMT_DATAFRAME_FUNCTIONS_TRANSFORM_HPP

#include <core/tag.hpp>
#include <core/utils.hpp>
#include <traits/frame_traits.hpp>
#include <traits/series_traits.hpp>

namespace amt {

namespace fn {

struct transform_t {
    template <Series SeriesType1, Series SeriesType2, typename Fn>
    constexpr SeriesType2 &operator()(SeriesType1 const &in, SeriesType2 &out,
                                      Fn &&fn) const {
        out.name(in.name());
        if constexpr(PureSeries<SeriesType2>){
            detail::set_dtype(out,in.dtype());
        }
        parallel_transform(in.begin(), in.end(), out.begin(),
                           std::forward<Fn>(fn));
        return out;
    }

    template <Series SeriesType1, Series SeriesType2, Series SeriesType3,
              typename Fn>
    constexpr SeriesType3 &operator()(SeriesType1 const &in1,
                                      SeriesType2 const &in2, SeriesType3 &out,
                                      Fn &&fn) const {
        out.name(in1.name());
        if constexpr(PureSeries<SeriesType3>){
            detail::set_dtype(out,in1.dtype());
        }
        parallel_transform(in1.begin(), in1.end(), in2.begin(), out.begin(),
                           std::forward<Fn>(fn));
        return out;
    }

    template <Series SeriesType, typename Fn>
    constexpr SeriesType operator()(SeriesType const &in, Fn &&fn) const {
        SeriesType temp(in.size());
        this->operator()(in, temp, std::forward<Fn>(fn));
        return temp;
    }

    template <Series SeriesType, typename Fn>
    constexpr SeriesType &operator()(SeriesType &in, tag::inplace_t, Fn &&fn) const {
        this->operator()(in, in, std::forward<Fn>(fn));
        return in;
    }

    template <Series SeriesType1, Series SeriesType2, typename Fn>
    constexpr auto operator()(SeriesType1 const &in1, SeriesType2 const &in2,
                              Fn &&fn) const {
        using return_type = series_result_t<SeriesType1, SeriesType2>;
        return_type temp(in1.size());
        this->operator()(in1, in2, temp, std::forward<Fn>(fn));
        return temp;
    }

    template <Series SeriesType1, Series SeriesType2, typename Fn>
    constexpr SeriesType1 &operator()(SeriesType1 &in1, SeriesType1 const &in2,
                                      tag::inplace_t, Fn &&fn) const {
        this->operator()(in1, in2, in1, std::forward<Fn>(fn));
        return in1;
    }

    template <Frame FrameType1, PureFrame FrameType2, typename Fn>
    constexpr FrameType2 &operator()(FrameType1 const &in, FrameType2 &out,
                                     Fn &&fn) const {
        frame_fn(in.begin(), in.end(), out.begin(), std::forward<Fn>(fn));
        return out;
    }

    template <Frame FrameType, typename Fn>
    constexpr FrameType operator()(FrameType const &in, Fn &&fn) const {
        FrameType temp(in.shape());
        temp.set_names(in);
        this->operator()(in, temp, std::forward<Fn>(fn));
        return temp;
    }

    template <PureFrame FrameType, typename Fn>
    constexpr FrameType &operator()(FrameType &in, tag::inplace_t, Fn &&fn) const {
        frame_fn(in.begin(), in.end(), in.begin(), std::forward<Fn>(fn));
        return in;
    }

    template <Frame FrameType1, Frame FrameType2, PureFrame FrameType3,
              typename Fn>
    constexpr FrameType3 &operator()(FrameType1 const &in1,
                                     FrameType2 const &in2, FrameType3 &out,
                                     Fn &&fn) const {

        frame_fn(in1.begin(), in1.end(), in2.begin(), out.begin(), std::forward<Fn>(fn));
        return out;
    }

    template <Frame FrameType1, Frame FrameType2, typename Fn>
    constexpr auto operator()(FrameType1 const &in1, FrameType2 const &in2,
                              Fn &&fn) const {
        using return_type = series_result_t<FrameType1, FrameType2>;
        return_type temp(in1.shape());
        temp.set_names(in1);
        this->operator()(in1, in2, temp, std::forward<Fn>(fn));
        return temp;
    }

    template <Frame FrameType1, Frame FrameType2, typename Fn>
    constexpr FrameType1 &operator()(FrameType1 &in1, FrameType1 const &in2,
                                     tag::inplace_t, Fn &&fn) const {
        frame_fn(in1.begin(), in1.end(), in2.begin(), in1.begin(), std::forward<Fn>(fn));
        return in1;
    }

  private:
    template <typename InputIt, typename OutputIt, typename Fn>
    constexpr void frame_fn(InputIt first, InputIt last, OutputIt out,
                            Fn &&fn) const {
        for (; first != last; ++first, ++out) {
            *out = std::invoke(std::forward<Fn>(fn), *first);
        }
    }

    template <typename InputIt1, typename InputIt2, typename OutputIt,
              typename Fn>
    constexpr void frame_fn(InputIt1 f1, InputIt1 l1, InputIt2 f2,
                                     OutputIt out, Fn &&fn) const {

        for (; f1 != l1; ++f1, ++f2, ++out) {
            *out = std::invoke(std::forward<Fn>(fn), *f1, *f2);
        }
    }
};

} // namespace fn

inline static constexpr auto transform = fn::transform_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_TRANSFORM_HPP
