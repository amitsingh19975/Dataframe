#if !defined(AMT_FUNCTIONS_DROP_COLS_HPP)
#define AMT_FUNCTIONS_DROP_COLS_HPP

#include <algorithm>
#include <core/visit.hpp>
#include <operator.hpp>
#include <unordered_set>

namespace amt::fn {

struct drop_cols_fn {

    template <FrameViewOrFrame F1, Frame F2>
    inline decltype(auto)
    operator()(F1 const &in, F2 &out,
               std::unordered_set<std::size_t> cids) const noexcept {
        auto cols = in.cols();
        auto rows = in.rows();

        std::erase_if(cids, [max = cols](auto i) { return i >= max; });

        out.resize(cols - cids.size(), rows);

        std::size_t k{};
        for (auto i = 0ul; i < cols; ++i) {
            if (cids.count(i) == 0) {
                out[k] = in[i];
                out.replace(k++, in.name(i));
            }
        }

        return static_cast<F2 &>(out);
    }

    template <FrameViewOrFrame F1, Frame F2>
    inline decltype(auto)
    operator()(F1 const &in, F2 &out,
               std::unordered_set<std::string_view> cname) const {
        std::unordered_set<std::size_t> cids;

        for (auto const &el : cname) {
            cids.insert(in.index(el));
        }

        return this->operator()(std::forward<F1>(in), std::forward<F2>(out),
                                std::move(cids));
    }

    template <Tag T, FrameViewOrFrame F>
    inline decltype(auto)
    operator()(F &&s, [[maybe_unused]] T t,
               std::unordered_set<std::size_t> cids) const noexcept {

        static_assert(
            !(is_view_v<std::decay_t<F>> && std::is_same_v<T, in_place_t>),
            "amt::fn::drop_cols_fn::operator(F&&,T,std::unordered_set<std::"
            "size_t>)"
            " : "
            "can not mutate view in place");

        result_type_t<F> temp;

        this->operator()(std::forward<F>(s), temp, std::move(cids));

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<F>(s);
        } else {
            return temp;
        }
    }

    template <Tag T, FrameViewOrFrame F>
    inline decltype(auto)
    operator()(F &&s, [[maybe_unused]] T t,
               std::unordered_set<std::string_view> cname) const noexcept {

        static_assert(
            !(is_view_v<std::decay_t<F>> && std::is_same_v<T, in_place_t>),
            "amt::fn::drop_cols_fn::operator(F&&,T,std::unordered_set<std::"
            "string_"
            "view>) : "
            "can not mutate view in place");

        result_type_t<F> temp;

        std::unordered_set<std::size_t> cids;

        for (auto const &el : cname) {
            cids.insert(s.index(el));
        }

        this->operator()(std::forward<F>(s), temp, std::move(cids));

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<F>(s);
        } else {
            return temp;
        }
    }

    template <Tag T, FrameViewOrFrame F, typename Fn>
    inline decltype(auto) operator()(F &&s, [[maybe_unused]] T t,
                                     Fn &&fn) const noexcept {

        static_assert(
            !(is_view_v<std::decay_t<F>> && std::is_same_v<T, in_place_t>),
            "amt::fn::drop_cols_fn::operator(F&&,T,std::unordered_set<std::"
            "string_"
            "view>) : "
            "can not mutate view in place");

        result_type_t<F> temp;

        std::unordered_set<std::size_t> cids;

        auto i = 0ul;
        for (auto const &ser : s) {
            if (fn(ser)) {
                cids.insert(i);
            }
            ++i;
        }

        this->operator()(std::forward<F>(s), temp, std::move(cids));

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<F>(s);
        } else {
            return temp;
        }
    }
};

} // namespace amt::fn

#endif // AMT_FUNCTIONS_DROP_COLS_HPP
