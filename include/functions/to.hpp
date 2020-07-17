#if !defined(AMT_FUNCTIONS_TO_HPP)
#define AMT_FUNCTIONS_TO_HPP

#include <algorithm>
#include <core/visit.hpp>
#include <operator.hpp>

namespace amt::fn {

template <typename To> struct to_fn {

    inline constexpr decltype(auto)
    operator()(Storage auto &&in, Storage auto &out) const noexcept {

        visit(in, [&](auto &&val) {
            using type = std::decay_t<decltype(val)>;

            if constexpr (std::is_same_v<To, std::string> &&
                          core::has_std_to_string_v<type>) {
                out = std::move(std::to_string(val));
            } else if constexpr (std::is_constructible_v<To, type>) {
                out = To(val);
            } else if constexpr (std::is_same_v<type, std::string>) {
                if constexpr (std::is_floating_point_v<To>) {
                    if constexpr (std::is_same_v<To, float>) {
                        out = std::stof(val);
                    } else if constexpr (std::is_same_v<To, double>) {
                        out = std::stod(val);
                    }
                } else if constexpr (std::is_integral_v<To>) {
                    if constexpr (std::is_same_v<To, int>) {
                        out = std::stoi(val);
                    } else if constexpr (std::is_same_v<To, long>) {
                        out = std::stol(val);
                    } else {
                        auto temp = std::stol(val);
                        out = static_cast<To>(temp);
                    }
                }
            }
        });
        return out;
    }

    template <Tag T>
    inline constexpr decltype(auto)
    operator()(Storage auto &&s, [[maybe_unused]] T t) const noexcept {
        using storage_type = std::decay_t<decltype(s)>;

        if constexpr (std::is_same_v<T, in_place_t>) {
            return this->operator()(s, s);
        } else {
            auto temp = storage_type{};
            this->operator()(s, temp);
            return temp;
        }
    }

    template <typename S1, typename S2>
    requires((Series<S1> ||
              SeriesView<S1>)&&(Series<S2> ||
                                SeriesView<S2>)) inline decltype(auto)
    operator()(S1 const &in, S2 &out) const noexcept {
        auto sz = in.size();

        if constexpr (!is_view_v<std::decay_t<S2>>) {
            out.resize(sz);
        }

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < sz; ++i) {
            decltype(auto) in_el = in[i];
            decltype(auto) out_el = out[i];
            this->operator()(in_el, out_el);
        }
        return static_cast<S2 &>(out);
    }

    template <Tag T, typename S>
    requires(Series<S> || SeriesView<S>) inline constexpr decltype(auto)
    operator()(S &&s, [[maybe_unused]] T t) const {

        if constexpr (std::is_same_v<T, in_place_t>) {
            this->operator()(std::forward<S>(s), std::forward<S>(s));
            return std::forward<S>(s);
        } else {
            result_type_t<S> temp;
            this->operator()(std::forward<S>(s), temp);
            return temp;
        }
    }

    template <typename F1, typename F2>
    requires((Frame<F1> ||
              FrameView<F1>)&&(Frame<F2> ||
                               FrameView<F2>)) inline decltype(auto)
    operator()(F1 const &in, F2 &out) const noexcept {
        auto cols = in.cols();
        auto rows = in.rows();
        using value_type = typename std::decay_t<F2>::value_type;

        if constexpr (!is_view_v<std::decay_t<F2>>) {
            out.resize(cols, value_type(rows));
        }

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < cols; ++i) {
            for (auto j = 0ul; j < rows; ++j) {
                decltype(auto) in_el = in[i][j];
                decltype(auto) out_el = out[i][j];
                this->operator()(in_el, out_el);
            }
        }
        return static_cast<F2 &>(out);
    }

    template <Tag T, typename F>
    requires(Frame<F> || FrameView<F>) inline constexpr decltype(auto)
    operator()(F &&s, [[maybe_unused]] T t) const {

        if constexpr (std::is_same_v<T, in_place_t>) {
            this->operator()(std::forward<F>(s), std::forward<F>(s));
            return std::forward<F>(s);
        } else {
            result_type_t<F> temp;

            this->operator()(std::forward<F>(s), temp);
            return temp;
        }
    }
};

} // namespace amt::fn

#endif // AMT_FUNCTIONS_TO_HPP
