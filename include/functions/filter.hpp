#if !defined(AMT_DATAFRAME_FUNCTIONS_FILTER_HPP)
#define AMT_DATAFRAME_FUNCTIONS_FILTER_HPP

#include <core/tag.hpp>
#include <functional>
#include <traits/frame_traits.hpp>
#include <traits/series_traits.hpp>
#include <series_utils.hpp>

namespace amt {

namespace fn {

struct filter_t {

    template <Series SeriesIn, PureSeries SeriesOut, typename Fn>
    requires(!Series<Fn>) constexpr SeriesOut &
    operator()(SeriesIn const &in, SeriesOut &out, Fn &&fn) const {
        auto out_it = std::back_inserter(out);
        out.name(in.name());
        detail::set_dtype(out,in.dtype());
        using_fn(in.begin(), in.end(), out_it, std::forward<Fn>(fn));
        return out;
    }

    template <Series SeriesType, typename Fn>
    requires(!Series<Fn>) constexpr series_result_t<SeriesType>
    operator()(SeriesType const &in, Fn &&fn) const {
        series_result_t<SeriesType> temp(in.name());
        this->operator()(in, temp, std::forward<Fn>(fn));
        return temp;
    }

    template <PureSeries SeriesType, typename Fn>
    requires(!Series<Fn>) constexpr SeriesType &
    operator()(SeriesType &in, tag::inplace_t, Fn &&fn) const {
        auto temp = this->operator()(in, std::forward<Fn>(fn));
        in = std::move(temp);
        return in;
    }

    template <Series SeriesIn, PureSeries SeriesOut, Series BoolSeries>
    constexpr SeriesOut &operator()(SeriesIn const &in, SeriesOut &out,
                                    BoolSeries const &bseries) const {
        if (bseries.size() != in.size()) {
            throw std::runtime_error(
                ERR_CSTR("amt::filter_t::operator(SeriesIn const &, SeriesOut "
                         "&,BoolSeries const&) : size mismatch"));
        }
        out.name(in.name());
        detail::set_dtype(out,in.dtype());
        auto out_it = std::back_inserter(out);
        using_bool(in.begin(), in.end(), out_it, bseries.begin());
        return out;
    }

    template <Series SeriesType, Series BoolSeries>
    constexpr series_result_t<SeriesType>
    operator()(SeriesType const &in, BoolSeries const &bseries) const {
        series_result_t<SeriesType> temp;
        this->operator()(in, temp, bseries);
        return temp;
    }

    template <Series SeriesType, Series BoolSeries>
    requires(!SeriesView<SeriesType>) constexpr SeriesType &
    operator()(SeriesType &in, tag::inplace_t, BoolSeries const &bseries) const {
        auto temp = this->operator()(in, bseries);
        in = std::move(temp);
        return in;
    }

    template <Frame FrameIn, PureFrame FrameOut, typename Predicate>
    constexpr FrameOut &
    operator()(FrameIn const &in, FrameOut &out, Predicate &&pred) const {
        if constexpr( is_series_v<Predicate> || is_frame_v<Predicate> ){
            using_bool(in, out, std::forward<Predicate>(pred));
        }else{
            using_fn(in, out, std::forward<Predicate>(pred));
        }
        return out;
    }

    template <Frame FrameType, typename Predicate>
    constexpr frame_result_t<FrameType>
    operator()(FrameType const &in, Predicate &&pred) const {
        frame_result_t<FrameType> temp;
        this->operator()(in, temp, std::forward<Predicate>(pred));
        return temp;
    }

    template <PureFrame FrameType, typename Predicate>
    constexpr FrameType&
    operator()(FrameType &in, tag::inplace_t, Predicate &&pred) const {
        auto temp = this->operator()(in, std::forward<Predicate>(pred));
        in = std::move(temp);
        return in;
    }

  private:
    template <typename InputIt, typename OutputIt, typename Fn>
    requires(!Frame<Fn> ||
             !Series<Fn>) constexpr void using_fn(InputIt first, InputIt last,
                                                  OutputIt out, Fn &&fn) const {
        std::size_t i = 0u;

        for (; first != last; ++first, ++i) {
            if constexpr (std::is_invocable_v<Fn, decltype(*first),
                                              std::size_t>) {
                if (std::invoke(std::forward<Fn>(fn), *first, i)) {
                    *out = *first;
                    ++out;
                }
            } else {
                if (std::invoke(std::forward<Fn>(fn), *first)) {
                    *out = *first;
                    ++out;
                }
            }
        }
    }

    template <typename InputIt, typename OutputIt, typename BoolIter>
    constexpr void using_bool(InputIt first, InputIt last, OutputIt out,
                              BoolIter biter) const {
        for (; first != last; ++first, ++biter) {
            if (*biter == true) {
                *out = *first;
                ++out;
            }
        }
    }

    template <Frame FrameIn, PureFrame FrameOut, typename Fn>
    requires(!Frame<Fn> || !Series<Fn>) constexpr void using_fn(
        FrameIn const &in, FrameOut &out, Fn &&fn) const {
        using value_type = typename FrameIn::value_type;

        value_type temp;
        out.resize(out.cols());
        out.set_names(in);

        for (auto i = 0u; i < in.rows(); ++i) {
            temp.resize(in.cols());
            for (auto j = 0u; j < in.cols(); ++j) {
                temp[j] = in[j][i];
            }
            if constexpr (std::is_invocable_v<Fn, decltype(in[i]),
                                              std::size_t>) {
                if (std::invoke(std::forward<Fn>(fn), temp, i)) {
                    out.row_push_back(std::move(temp));
                }
            } else {
                if (std::invoke(std::forward<Fn>(fn), temp)) {
                    out.row_push_back(std::move(temp));
                }
            }
        }
    }

    template <Frame FrameIn, PureFrame FrameOut, Frame BoolFrame>
    constexpr void using_bool(FrameIn const &in, FrameOut &out,
                              BoolFrame const &bf) const {
        if (in.shape() != bf.shape()) {
            throw std::length_error(
                ERR_CSTR("amt::filter_t::using_bool(FrameIn const&, FrameOut&, "
                         "BoolFrame const&) : shape mismatch"));
        }

        out.resize(out.cols());
        out.set_names(in);

        for (auto i = 0u; i < in.rows(); ++i) {
            bool should_be_inserted = false;
            for (auto j = 0u; j < bf.cols(); ++j) {
                should_be_inserted = (should_be_inserted || bf[j][i]);
            }

            if (should_be_inserted) {
                for (auto j = 0u; j < in.cols(); ++j) {
                    out[j].push_back(in[i][j]);
                }
            }
        }
    }

    template <Frame FrameIn, PureFrame FrameOut, Series BoolSeries>
    constexpr void using_bool(FrameIn const &in, FrameOut &out,
                              BoolSeries const &bf) const {
        if (in.rows() != bf.size()) {
            throw std::length_error(
                ERR_CSTR("amt::filter_t::using_bool(FrameIn const&, FrameOut&, "
                         "BoolFrame const&) : rows mismatch"));
        }

        out.resize(out.cols());
        out.set_names(in);

        for (auto i = 0u; i < in.rows(); ++i) {
            bool should_be_inserted = bf[i];

            if (should_be_inserted) {
                for (auto j = 0u; j < in.cols(); ++j) {
                    out[j].push_back(in[i][j]);
                }
            }
        }
    }
};

} // namespace fn

inline static constexpr auto filter = fn::filter_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_FILTER_HPP
