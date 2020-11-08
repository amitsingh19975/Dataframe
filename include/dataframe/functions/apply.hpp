#if !defined(AMT_DATAFRAME_FUNCTIONS_APPLY_HPP)
#define AMT_DATAFRAME_FUNCTIONS_APPLY_HPP

#include <dataframe/core/utils.hpp>
#include <dataframe/traits/frame_traits.hpp>
#include <dataframe/traits/series_traits.hpp>

namespace amt {

namespace fn {

struct apply_t {

    template <Series SeriesType, Series SeriesOut, typename Fn>
    constexpr SeriesOut &operator()(Fn &&fn, SeriesType const &in,
                                    SeriesOut &out) const {
        return std::invoke(std::forward<Fn>(fn), in, out);
    }

    template <Series SeriesType, typename Fn>
    constexpr decltype(auto) operator()(Fn &&fn, SeriesType const &in) const {
        return std::invoke(std::forward<Fn>(fn), in);
    }

    template <Series SeriesType, typename Fn>
    constexpr decltype(auto) operator()(Fn &&fn, SeriesType const &in,
                                        tag::inplace_t) const {
        return std::invoke(std::forward<Fn>(fn), in, tag::inplace_t{});
    }

    template <Frame FrameType, Frame FrameOut, typename Fn>
    constexpr decltype(auto) operator()(Fn &&fn, FrameType const &in,
                                        FrameOut &out) const {
        return std::invoke(std::forward<Fn>(fn), in, out);
    }

    template <Frame FrameType, Frame FrameOut, typename Fn, typename... Args>
    requires(HasModifiesShapeTrait<Fn> &&
             !std::decay_t<Fn>::modifies_shape) FrameOut &
    operator()(Fn &&fn, FrameType const &in, FrameOut &out, index_list list,
               Args &&... args) const {
        normalize_list(list, in.rows());
        auto it = list.begin();
        out.resize(in.cols());

        for (auto i = 0ul; i < in.cols(); ++i) {
            if (it != list.end() && i == *it) {
                ++it;
                std::invoke(std::forward<Fn>(fn), in[i], out[i],
                            std::forward<Args>(args)...);
            } else {
                out[i] = in[i];
            }
        }
        return out;
    }

    template <Frame FrameType, Frame FrameOut, typename Fn, typename... Args>
    FrameOut &operator()(Fn &&fn, FrameType const &in, FrameOut &out,
                         name_list const &list, Args &&... args) const {
        index_list id;
        for (auto i = 0ul; i < in.cols(); ++i) {
            if (auto it = std::find(list.begin(), list.end(), in[i].name());
                it != list.end()) {
                id.push_back(i);
            }
        }
        return this->operator()(std::forward<Fn>(fn), in, out, std::move(id),
                                std::forward<Args>(args)...);
    }

    template <Frame FrameType, typename Fn, typename... Args>
    frame_result_t<FrameType> operator()(Fn &&fn, FrameType const &in,
                                         index_list list,
                                         Args &&... args) const {
        frame_result_t<FrameType> out;
        this->operator()(std::forward<Fn>(fn), in, out, std::move(list),
                         std::forward<Args>(args)...);
        return out;
    }

    template <Frame FrameType, typename Fn, typename... Args>
    frame_result_t<FrameType> operator()(Fn &&fn, FrameType const &in,
                                         name_list const &list,
                                         Args &&... args) const {
        frame_result_t<FrameType> out;
        this->operator()(std::forward<Fn>(fn), in, out, list,
                         std::forward<Args>(args)...);
        return out;
    }

    template <Frame FrameType, typename Fn, typename... Args>
    FrameType &operator()(Fn &&fn, FrameType &in, tag::inplace_t,
                          index_list list, Args &&... args) const {
        auto out = this->operator()(std::forward<Fn>(fn), in, std::move(list),
                                    std::forward<Args>(args)...);
        in = std::move(out);
        return in;
    }

    template <Frame FrameType, typename Fn, typename... Args>
    FrameType &operator()(Fn &&fn, FrameType &in, tag::inplace_t,
                          name_list const &list, Args &&... args) const {
        auto out = this->operator()(std::forward<Fn>(fn), in, list,
                                    std::forward<Args>(args)...);
        in = std::move(out);
        return in;
    }
};

} // namespace fn

inline static constexpr auto apply = fn::apply_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_APPLY_HPP
