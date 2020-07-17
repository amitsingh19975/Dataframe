#if !defined(AMT_FUNCTIONS_FILTER_HPP)
#define AMT_FUNCTIONS_FILTER_HPP

#include <algorithm>
#include <core/visit.hpp>
#include <operator.hpp>

namespace amt::fn {

struct filter_fn {

  private:
    template <typename Fn>
    inline constexpr bool check(Storage auto &&in, Fn &&fn) const noexcept {

        using storage_type = std::decay_t<decltype(in)>;
        using type_list = typename storage_type::type_list;

        bool flag = false;
        visit(in, [&, fn = std::move(fn)](auto &&val) {
            using type = std::decay_t<decltype(val)>;

            if constexpr (core::is_generic_unary_v<type_list, Fn>) {
                if (fn(val))
                    flag = true;
            } else {
                using traits = core::function_traits<Fn>;
                using param_type =
                    std::decay_t<typename traits::template arg<0>::type>;
                using nparam_type = core::norm_type_t<param_type>;
                if constexpr (std::is_same_v<type, nparam_type>) {
                    if (fn(val))
                        flag = true;
                }
            }
        });
        return flag;
    }

  public:
    template <typename Fn>
    inline constexpr decltype(auto)
    operator()(Storage auto &&in, Storage auto &out, Fn &&fn) const noexcept {

        if (!check(in, std::move(fn))) {
            out = std::move(in);
        }
        return out;
    }

    template <Tag T, typename Fn>
    inline constexpr decltype(auto)
    operator()(Storage auto &&s, [[maybe_unused]] T t, Fn &&fn) const noexcept {
        using storage_type = std::decay_t<decltype(s)>;

        auto temp = storage_type{};
        this->operator()(s, temp, std::forward<Fn>(fn));

        if constexpr (std::is_same_v<T, in_place_t>) {
            this->operator()(s, temp, std::forward<Fn>(fn));
            s = std::move(temp);
            return s;
        } else {
            return temp;
        }
    }

    template <typename S1, typename S2, typename Fn>
    requires((Series<S1> || SeriesView<S1>)&&Series<S2>) inline decltype(auto)
    operator()(S1 const &in, S2 &out, Fn &&fn) const noexcept {
        auto sz = in.size();

        for (auto i = 0ul; i < sz; ++i) {
            decltype(auto) in_el = in[i];
            if (!check(in_el, fn)) {
                out.push_back(in_el);
            }
        }
        return static_cast<S2 &>(out);
    }

    template <Tag T, typename S, typename Fn>
    requires(Series<S> || SeriesView<S>) inline constexpr decltype(auto)
    operator()(S &&s, [[maybe_unused]] T t, Fn &&fn) const {

        static_assert(
            !(is_view_v<std::decay_t<S>> && std::is_same_v<T, in_place_t>),
            "amt::fn::filer::operator(S&&,T,Fn) : "
            "View can not be filtered in place");

        result_type_t<S> temp;

        this->operator()(std::forward<S>(s), temp, std::forward<Fn>(fn));

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<S>(s);
        } else {
            return temp;
        }
    }

    template <typename F1, typename F2, typename Fn>
    requires((Frame<F1> || FrameView<F1>)&&Frame<F2>) inline decltype(auto)
    operator()(F1 const &in, F2 &out, Fn &&fn) const noexcept {
        auto cols = in.cols();
        auto rows = in.rows();

        out = in;
        std::vector<std::size_t> ids;

        for (auto j = 0ul; j < rows; ++j) {
            for (auto i = 0ul; i < cols; ++i) {
                decltype(auto) in_el = in[i][j];
                if (check(in_el, fn)) {
                    ids.push_back(j);
                    break;
                }
            }
        }

        std::size_t i{};
        for (auto const &el : ids) {
            out.erase_row(el - i++);
        }

        return static_cast<F2 &>(out);
    }

    template <Tag T, typename F, typename Fn>
    requires(Frame<F> || FrameView<F>) inline constexpr decltype(auto)
    operator()(F &&s, [[maybe_unused]] T t, Fn &&fn) const {

        static_assert(
            !(is_view_v<std::decay_t<F>> && std::is_same_v<T, in_place_t>),
            "amt::fn::filer::operator(F&&,T,Fn) : "
            "View can not be filtered in place");

        result_type_t<F> temp;

        this->operator()(std::forward<F>(s), temp, std::forward<Fn>(fn));

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<F>(s);
        } else {
            return temp;
        }
    }
};

} // namespace amt::fn

#endif // AMT_FUNCTIONS_FILTER_HPP
