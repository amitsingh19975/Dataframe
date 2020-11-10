#if !defined(AMT_DATAFRAME_FUNCTIONS_DROP_HPP)
#define AMT_DATAFRAME_FUNCTIONS_DROP_HPP

#include <cmath>
#include <dataframe/functions/filter.hpp>

namespace amt {

namespace fn {

struct drop_if_t {
    template <Series SeriesTypeIn, PureSeries SeriesTypeOut, typename Fn>
    constexpr SeriesTypeOut &operator()(SeriesTypeIn const &in,
                                        SeriesTypeOut &out, Fn &&fn) const {
        filter(in, out, [fn = std::forward<Fn>(fn)](auto const &val) {
            return !visit(val, fn);
        });
        return out;
    }

    template <Series SeriesType, typename Fn>
    constexpr series_result_t<SeriesType> operator()(SeriesType const &in,
                                                     Fn &&fn) const {
        series_result_t<SeriesType> temp;
        this->operator()(in, temp, std::forward<Fn>(fn));
        return temp;
    }

    template <PureSeries SeriesType, typename Fn>
    constexpr SeriesType &operator()(SeriesType &in, tag::inplace_t,
                                     Fn &&fn) const {
        auto temp = this->operator()(in, std::forward<Fn>(fn));
        in = std::move(temp);
        return temp;
    }

    template <Frame FrameTypeIn, PureFrame FrameTypeOut, typename Fn>
    constexpr FrameTypeOut &operator()(FrameTypeIn const &in, FrameTypeOut &out,
                                       Fn &&fn) const {
        filter(in, out, [fn = std::forward<Fn>(fn)](auto const &s) {
            return !std::invoke(fn, s);
        });
        return out;
    }

    template <Frame FrameTypeIn, typename Fn>
    constexpr frame_result_t<FrameTypeIn> operator()(FrameTypeIn const &in,
                                                     Fn &&fn) const {
        frame_result_t<FrameTypeIn> temp;
        this->operator()(in, temp, std::forward<Fn>(fn));
        return temp;
    }

    template <PureFrame FrameTypeIn, typename Fn>
    constexpr FrameTypeIn &operator()(FrameTypeIn &in, tag::inplace_t,
                                      Fn &&fn) const {
        auto temp = this->operator()(in, std::forward<Fn>(fn));
        in = std::move(temp);
        return in;
    }
};

struct drop_nan_t {

    template <Series SeriesType>
    constexpr SeriesType &operator()(SeriesType const &in,
                                     SeriesType &out) const {
        drop_if_t{}(in, out, []<typename T>(T const &val) {
            if constexpr (std::numeric_limits<T>::has_quiet_NaN) {
                return std::isnan(val);
            } else {
                return false;
            }
        });
        return out;
    }

    template <Series SeriesType>
    constexpr SeriesType operator()(SeriesType const &in) const {
        SeriesType temp(in.name());
        this->operator()(in, temp);
        return temp;
    }

    template <PureSeries SeriesType>
    constexpr SeriesType &operator()(SeriesType &in, tag::inplace_t) const {
        auto temp = this->operator()(in);
        in = std::move(temp);
        return in;
    }

    template <Frame FrameTypeIn, PureFrame FrameTypeOut>
    constexpr FrameTypeOut &operator()(FrameTypeIn const &in,
                                       FrameTypeOut &out) const {
        drop_if_t{}(in, out, [](auto const &s) {
            for (auto const &el : s) {
                auto temp = visit(el, []<typename T>(T const &val) {
                    if constexpr (std::numeric_limits<T>::has_quiet_NaN) {
                        return std::isnan(val);
                    } else {
                        return false;
                    }
                });
                if (temp)
                    return true;
            }
            return false;
        });
        return out;
    }

    template <Frame FrameTypeIn>
    constexpr frame_result_t<FrameTypeIn>
    operator()(FrameTypeIn const &in) const {
        frame_result_t<FrameTypeIn> temp;
        this->operator()(in, temp);
        return temp;
    }

    template <PureFrame FrameTypeIn>
    constexpr FrameTypeIn &operator()(FrameTypeIn &in, tag::inplace_t) const {
        auto temp = this->operator()(in);
        in = std::move(temp);
        return in;
    }
};

struct drop_row_t {

    template <Series SeriesType>
    SeriesType &operator()(SeriesType const &in, SeriesType &out,
                           index_list list) const {
        normalize_list(list, in.rows());
        std::size_t i{};
        auto it = list.begin();
        drop_if_t{}(in, out,
                    [&i, &it, end = list.end()]<typename T>(T const &) {
                        if ((it != end) && i == *it) {
                            ++it;
                            ++i;
                            return true;
                        }
                        ++i;
                        return false;
                    });
        return out;
    }

    template <Series SeriesType>
    SeriesType operator()(SeriesType const &in, index_list list) const {
        SeriesType temp(in.name());
        this->operator()(in, temp, std::move(list));
        return temp;
    }

    template <PureSeries SeriesType>
    SeriesType &operator()(SeriesType &in, tag::inplace_t,
                           index_list list) const {
        auto temp = this->operator()(in, std::move(list));
        in = std::move(temp);
        return in;
    }

    template <Series SeriesType>
    SeriesType &
    operator()(SeriesType const &in, SeriesType &out, std::size_t s,
               std::size_t e = std::numeric_limits<std::size_t>::max()) const {
        index_list ids;
        if (s > e) {
            throw std::runtime_error(ERR_CSTR(
                "amt::drop_row::operator()(SeriesType const &, SeriesType "
                "&,std::size_t, std::size_t) : start > end"));
        }
        if (s == e)
            return out;

        if (e == std::numeric_limits<std::size_t>::max()) {
            e = in.size();
        }

        ids.resize(e - s);
        std::fill(ids.begin(), ids.end(), s);
        return this->operator()(in, out, ids);
    }

    template <Series SeriesType>
    SeriesType
    operator()(SeriesType const &in, std::size_t s,
               std::size_t e = std::numeric_limits<std::size_t>::max()) const {
        SeriesType temp(in.name());
        this->operator()(in, temp, s, e);
        return temp;
    }

    template <PureSeries SeriesType>
    SeriesType &
    operator()(SeriesType &in, tag::inplace_t, std::size_t s,
               std::size_t e = std::numeric_limits<std::size_t>::max()) const {
        auto temp = this->operator()(in, s, e);
        in = std::move(temp);
        return in;
    }

    template <Frame FrameTypeIn, PureFrame FrameTypeOut>
    FrameTypeOut &operator()(FrameTypeIn const &in, FrameTypeOut &out,
                             index_list list) const {
        normalize_list(list, in.rows());
        auto it = list.begin();
        auto sz = in.rows() - list.size();
        out = FrameTypeOut(in.cols(), sz);
        out.set_names(in);

        auto k = 0u;
        for (auto i = 0u; i < in.rows(); ++i) {
            if ((it != list.end()) && *it == i) {
                ++it;
                continue;
            }
            for (auto j = 0u; j < in.cols(); ++j) {
                out[j][k] = in[j][i];
            }
            ++k;
        }
        return out;
    }

    template <Frame FrameTypeIn>
    frame_result_t<FrameTypeIn> operator()(FrameTypeIn const &in,
                                           index_list list) const {
        frame_result_t<FrameTypeIn> temp;
        this->operator()(in, temp, std::move(list));
        return temp;
    }

    template <PureFrame FrameTypeIn>
    FrameTypeIn &operator()(FrameTypeIn &in, tag::inplace_t,
                            index_list list) const {
        auto temp = this->operator()(in, std::move(list));
        in = std::move(temp);
        return in;
    }

    template <Frame FrameType>
    FrameType &
    operator()(FrameType const &in, FrameType &out, std::size_t s,
               std::size_t e = std::numeric_limits<std::size_t>::max()) const {
        index_list ids;
        if (s > e) {
            throw std::runtime_error(ERR_CSTR(
                "amt::drop_row::operator()(FrameType const &, FrameType "
                "&,std::size_t, std::size_t) : start > end"));
        }
        if (s == e)
            return out;

        if (e == std::numeric_limits<std::size_t>::max()) {
            e = in.rows();
        }

        ids.resize(e - s);
        std::fill(ids.begin(), ids.end(), s);
        return this->operator()(in, out, ids);
    }

    template <Frame FrameTypeIn>
    frame_result_t<FrameTypeIn>
    operator()(FrameTypeIn const &in, std::size_t s,
               std::size_t e = std::numeric_limits<std::size_t>::max()) const {
        frame_result_t<FrameTypeIn> temp;
        this->operator()(in, temp, s, e);
        return temp;
    }

    template <PureFrame FrameTypeIn>
    FrameTypeIn &
    operator()(FrameTypeIn &in, tag::inplace_t, std::size_t s,
               std::size_t e = std::numeric_limits<std::size_t>::max()) const {
        auto temp = this->operator()(in, s, e);
        in = std::move(temp);
        return in;
    }
};

struct drop_col_t {

    template <Frame FrameTypeIn, PureFrame FrameTypeOut>
    FrameTypeOut &operator()(FrameTypeIn const &in, FrameTypeOut &out,
                             name_list list) const {
        index_list i;
        i.reserve(list.size());
        auto names = in.vnames();
        for (auto const &el : list) {
            auto it = std::find(names.begin(), names.end(), el);
            i.push_back(
                static_cast<std::size_t>(std::distance(names.begin(), it)));
        }
        this->operator()(in, out, std::move(i));
        return out;
    }

    template <Frame FrameTypeIn, PureFrame FrameTypeOut>
    FrameTypeOut &operator()(FrameTypeIn const &in, FrameTypeOut &out,
                             index_list list) const {
        normalize_list(list, in.cols());
        auto it = list.begin();
        auto sz = in.cols() - list.size();
        out = FrameTypeOut(sz);

        auto k = 0u;
        for (auto i = 0u; i < in.cols(); ++i) {
            if (it != list.end() && (i == *it)) {
                ++it;
                continue;
            }
            out[k++] = in[i];
        }
        return out;
    }

    template <Frame FrameTypeIn, typename List>
    requires(is_name_list_v<List> ||
             is_index_list_v<List>) frame_result_t<FrameTypeIn>
    operator()(FrameTypeIn const &in, List list) const {
        frame_result_t<FrameTypeIn> temp;
        this->operator()(in, temp, std::move(list));
        return temp;
    }

    template <PureFrame FrameTypeIn, typename List>
    requires(is_name_list_v<List> || is_index_list_v<List>) FrameTypeIn &
    operator()(FrameTypeIn &in, tag::inplace_t, List list) const {
        auto temp = this->operator()(in, std::move(list));
        in = std::move(temp);
        return in;
    }
};

struct drop_none_t {

    template <Series SeriesType>
    constexpr SeriesType &operator()(SeriesType const &in,
                                     SeriesType &out) const {
        drop_if_t{}(in, out,
                    []<typename T>(T const &) { return is_monostate_v<T>; });
        return out;
    }

    template <Series SeriesType>
    constexpr SeriesType operator()(SeriesType const &in) const {
        SeriesType temp(in.name());
        this->operator()(in, temp);
        return temp;
    }

    template <PureSeries SeriesType>
    constexpr SeriesType &operator()(SeriesType &in, tag::inplace_t) const {
        auto temp = this->operator()(in);
        in = std::move(temp);
        return in;
    }
    template <Frame FrameTypeIn, PureFrame FrameTypeOut>
    constexpr FrameTypeOut &operator()(FrameTypeIn const &in,
                                       FrameTypeOut &out) const {
        drop_if_t{}(in, out, [](auto const &s) {
            for (auto const &el : s) {
                if (is_none(el))
                    return true;
            }
            return false;
        });
        return out;
    }

    template <Frame FrameTypeIn>
    constexpr frame_result_t<FrameTypeIn>
    operator()(FrameTypeIn const &in) const {
        frame_result_t<FrameTypeIn> temp;
        this->operator()(in, temp);
        return temp;
    }

    template <PureFrame FrameTypeIn>
    constexpr FrameTypeIn &operator()(FrameTypeIn &in, tag::inplace_t) const {
        auto temp = this->operator()(in);
        in = std::move(temp);
        return in;
    }
};

} // namespace fn

inline static constexpr auto drop_if = fn::drop_if_t{};
inline static constexpr auto drop_nan = fn::drop_nan_t{};
inline static constexpr auto drop_row = fn::drop_row_t{};
inline static constexpr auto drop_col = fn::drop_col_t{};
inline static constexpr auto drop_none = fn::drop_none_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_DROP_HPP
