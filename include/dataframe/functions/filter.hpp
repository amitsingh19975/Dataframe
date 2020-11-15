#if !defined(AMT_DATAFRAME_FUNCTIONS_FILTER_HPP)
#define AMT_DATAFRAME_FUNCTIONS_FILTER_HPP

#include <dataframe/core/compare.hpp>
#include <dataframe/core/tag.hpp>
#include <dataframe/core/column.hpp>
#include <dataframe/series_utils.hpp>
#include <dataframe/traits/frame_traits.hpp>
#include <dataframe/traits/series_traits.hpp>
#include <functional>

namespace amt {

namespace fn {

template <std::size_t N> struct filter_col_t : column_t<N> {

    using super_type = column_t<N>;

    template <typename... Ts>
    constexpr filter_col_t(Ts &&... args) noexcept
        : super_type(std::forward<Ts>(args)...) {}

    template <Frame FrameType, PureFrame FrameOut, typename Fn>
    constexpr FrameOut &operator()(FrameType const &in, FrameOut &out,
                                   Fn && fn) const {
        
        out.set_names(in);
        out.set_dtypes(in);

        auto ids = super_type::get_indices(in);

        for (auto i = 0ul; i < in.rows(); ++i) {
            if constexpr( !HasStoreResult<Fn> ){
                auto const& s = ids[0];
                auto const& el = s[i];
                if (std::invoke(std::forward<Fn>(fn), el)) {
                    out.row_push_back(in.get_row(i));
                }

            }else{
                for(auto j = 0ul; j < ids.size(); ++j){
                    auto const &el = in[ids[j]][i];
                    fn.store_result(j,el);
                }

                if (std::invoke(std::forward<Fn>(fn))) {
                    out.row_push_back(in.get_row(i));
                }
            }

        }
        
        return out;
    }

    template <Frame FrameType, typename Fn>
    constexpr frame_result_t<FrameType> operator()(FrameType const &in,
                                                   Fn &&fn) const {
        frame_result_t<FrameType> out(in.cols());
        this->operator()(in, out, std::forward<Fn>(fn));
        return out;
    }

    template <PureFrame FrameType, typename Fn>
    constexpr FrameType &operator()(FrameType &in, tag::inplace_t,
                                    Fn &&fn) const {
        auto out = this->operator()(in, std::forward<Fn>(fn));
        in = std::move(out);
        return in;
    }
};

template <typename... Ts> filter_col_t(Ts &&...) -> filter_col_t<sizeof...(Ts)>;
struct filter_t {

    template <Series SeriesIn, PureSeries SeriesOut, typename Fn>
    requires(!Series<Fn>) constexpr SeriesOut &
    operator()(SeriesIn const &in, SeriesOut &out, Fn &&fn) const {
        auto out_it = std::back_inserter(out);
        out.name(in.name());
        detail::set_dtype(out, in.dtype());
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
        detail::set_dtype(out, in.dtype());
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
    operator()(SeriesType &in, tag::inplace_t,
               BoolSeries const &bseries) const {
        auto temp = this->operator()(in, bseries);
        in = std::move(temp);
        return in;
    }

    template <Frame FrameIn, PureFrame FrameOut, typename Predicate>
    constexpr FrameOut &operator()(FrameIn const &in, FrameOut &out,
                                   Predicate &&pred) const {
        if constexpr (is_series_v<Predicate> || is_frame_v<Predicate>) {
            using_bool(in, out, std::forward<Predicate>(pred));
        } else {
            using_fn(in, out, std::forward<Predicate>(pred));
        }
        out.set_names(in);
        out.set_dtypes(in);
        return out;
    }

    template <Frame FrameType, typename Predicate>
    constexpr frame_result_t<FrameType> operator()(FrameType const &in,
                                                   Predicate &&pred) const {
        frame_result_t<FrameType> temp;
        this->operator()(in, temp, std::forward<Predicate>(pred));
        return temp;
    }

    template <PureFrame FrameType, typename Predicate>
    constexpr FrameType &operator()(FrameType &in, tag::inplace_t,
                                    Predicate &&pred) const {
        auto temp = this->operator()(in, std::forward<Predicate>(pred));
        in = std::move(temp);
        return in;
    }

    template <typename T>
    constexpr filter_col_t<1> operator[](T &&key) const noexcept {
        return {std::forward<T>(key)};
    }

    template <typename... Args>
    requires( (ColumnKey<Args> && ... && (sizeof...(Args) > 0ul) ) )
    constexpr filter_col_t<sizeof...(Args)> operator()(Args &&... args) const noexcept {
        return {std::forward<Args>(args)...};
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

        out.resize(in.cols());
        out.set_names(in);
        out.set_dtypes(in);

        for (auto i = 0u; i < in.rows(); ++i) {
            auto temp = in.get_row(i);
            if constexpr (std::is_invocable_v<Fn, decltype(in[i]),
                                              std::size_t>) {
                if (std::invoke(std::forward<Fn>(fn), temp, i)) {
                    out.row_push_back(temp);
                }
            } else {
                if (std::invoke(std::forward<Fn>(fn), temp)) {
                    out.row_push_back(temp);
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

        out.resize(in.cols());
        out.set_names(in);
        out.set_dtypes(in);

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

        out.resize(in.cols());
        out.set_names(in);
        out.set_dtypes(in);

        for (auto i = 0u; i < in.rows(); ++i) {
            bool should_be_inserted = bf[i];

            if (should_be_inserted) {
                out.row_push_back(in.get_row(i));
            }
        }
    }
};

struct list_filter_t {
    template <Frame FrameIn, PureFrame FrameOut, typename List>
    requires(is_index_list_v<List> || is_name_list_v<List>) constexpr FrameOut &
    operator()(FrameIn const &in, FrameOut &out, List &&list) const {
        helper(in, out, std::forward<List>(list));
        return out;
    }

    template <Frame FrameIn, typename List>
    requires(is_index_list_v<List> ||
             is_name_list_v<List>) constexpr frame_result_t<FrameIn>
    operator()(FrameIn const &in, List &&list) const {
        frame_result_t<FrameIn> out;
        this->operator()(in, out, std::forward<List>(list));
        return out;
    }

    template <PureFrame FrameIn, typename List>
    requires(is_index_list_v<List> || is_name_list_v<List>) constexpr FrameIn &
    operator()(FrameIn const &in, tag::inplace_t, List &&list) const {
        auto temp = this->operator()(in, std::forward<List>(list));
        in = std::move(temp);
        return in;
    }

  private:
    template <Frame FrameIn, PureFrame FrameOut, typename List>
    requires(is_index_list_v<List>) constexpr void helper(FrameIn const &in,
                                                          FrameOut &out,
                                                          List list) const {
        normalize_list(list, in.cols());
        out.resize(list.size());
        auto k = 0u;
        for (auto const &idx : list) {
            out[k++] = in[idx];
        }
    }

    template <Frame FrameIn, PureFrame FrameOut, typename List>
    requires(is_name_list_v<List>) void helper(FrameIn const &in, FrameOut &out,
                                               List const &list) const {
        index_list i;
        i.reserve(list.size());
        auto names = in.vnames();
        for (auto const &el : list) {
            auto it = std::find(names.begin(), names.end(), el);
            i.push_back(
                static_cast<std::size_t>(std::distance(names.begin(), it)));
        }
        helper(in, out, std::move(i));
    }
};

} // namespace fn

inline static constexpr auto list_filter = fn::list_filter_t{};

inline static constexpr auto filter = fn::filter_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_FILTER_HPP
