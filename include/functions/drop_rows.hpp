#if !defined(AMT_FUNCTIONS_DROP_ROWS_HPP)
#define AMT_FUNCTIONS_DROP_ROWS_HPP

#include <algorithm>
#include <core/visit.hpp>
#include <operator.hpp>
#include <unordered_set>

namespace amt::fn {

struct drop_rows_fn {

    template <typename S1, typename S2>
    requires((Series<S1> || SeriesView<S1>)&&Series<S2>) inline decltype(auto)
    operator()(S1 const &in, S2 &out,
               std::unordered_set<std::size_t> ids) const noexcept {

        std::erase_if(ids, [max = in.size()](auto i) { return i >= max; });
        out.resize(in.size() - ids.size());

        std::size_t j{};

        for (auto i = 0ul; i < in.size(); ++i) {
            if (ids.count(i) == 0) {
                out[j++] = in[i];
            }
        }

        return static_cast<S2 &>(out);
    }

    template <Tag T, typename S>
    requires(Series<S> || SeriesView<S>) inline decltype(auto)
    operator()(S &&s, [[maybe_unused]] T t,
               std::unordered_set<std::size_t> ids) const noexcept {

        static_assert(
            !(is_view_v<std::decay_t<S>> && std::is_same_v<T, in_place_t>),
            "amt::fn::drop_rows_fn::operator(S&&,T,std::unordered_set<std::"
            "size_t>)"
            " : "
            "can not mutate view in place");

        result_type_t<S> temp;

        this->operator()(std::forward<S>(s), temp, std::move(ids));

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<S>(s);
        } else {
            return temp;
        }
    }

    template <typename F1, typename F2>
    requires((Frame<F1> || FrameView<F1>)&&Frame<F2>) inline decltype(auto)
    operator()(F1 const &in, F2 &out,
               std::unordered_set<std::size_t> rids) const noexcept {
        auto cols = in.cols();
        auto rows = in.rows();

        out.resize(rows - rids.size(), cols);

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < cols; ++i) {
            std::size_t k{};
            for (auto j = 0ul; j < rows; ++j) {
                if (rids.count(j) == 0) {
                    out[i][k++] = in[i][j];
                }
            }
        }

        return static_cast<F2 &>(out);
    }

    template <Tag T, typename F>
    requires(Frame<F> || FrameView<F>) inline decltype(auto)
    operator()(F &&s, [[maybe_unused]] T t,
               std::unordered_set<std::size_t> rids) const noexcept {

        static_assert(
            !(is_view_v<std::decay_t<F>> && std::is_same_v<T, in_place_t>),
            "amt::fn::drop_rows_fn::operator(F&&,T,std::unordered_set<std::"
            "size_t>)"
            " : "
            "can not mutate view in place");

        result_type_t<F> temp;

        this->operator()(std::forward<F>(s), temp, std::move(rids));

        temp.set_name(s.name_to_vector());

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<F>(s);
        } else {
            return temp;
        }
    }
};

} // namespace amt::fn

#endif // AMT_FUNCTIONS_DROP_ROWS_HPP
