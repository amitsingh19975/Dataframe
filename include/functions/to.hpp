#if !defined(AMT_FUNCTIONS_TO_HPP)
#define AMT_FUNCTIONS_TO_HPP

#include <algorithm>
#include <core/visit.hpp>
#include <operator.hpp>
#include <boost/lexical_cast.hpp>

namespace amt::fn {

template <typename To> struct to_fn {

    inline constexpr decltype(auto) operator()(Box auto &&in, Box auto &out,
                                               bool &check) const noexcept {

        visit(in, [&]<typename T>(T const& val) {
            using type = std::decay_t<T>;

            if constexpr (std::is_same_v<To, std::string> &&
                          core::has_std_to_string_v<type>) {
                out = std::move(std::to_string(val));
            } else if constexpr (std::is_constructible_v<To, type>) {
                out = To(val);
            } else if constexpr (std::is_same_v<type, std::string>) {
                try {
                    out = boost::lexical_cast<To>(val);
                } catch (...) {
                    check = true;
                }
            }
        });

        return out;
    }

    inline constexpr decltype(auto) operator()(Box auto &&in,
                                               Box auto &out) const noexcept {
        auto check = false;
        this->operator()(in, out, check);
        if (check)
            out = in;
        return out;
    }

    template <Tag T>
    inline constexpr decltype(auto)
    operator()(Box auto &&s, [[maybe_unused]] T t) const noexcept {
        using storage_type = std::decay_t<decltype(s)>;

        if constexpr (std::is_same_v<T, in_place_t>) {
            return this->operator()(s, s);
        } else {
            auto temp = storage_type{};
            this->operator()(s, temp);
            return temp;
        }
    }

    template <SeriesViewOrSeries S1, Series S2>
    inline decltype(auto) operator()(S1 const &in, S2 &out) const noexcept {
        auto sz = in.size();
        out.resize(sz);

        out.name() = in.name();

        for (auto i = 0ul; i < sz; ++i) {
            bool fail = false;
            auto const& in_el = in[i];
            auto& out_el = out[i];
            this->operator()(in_el, out_el, fail);
            if (fail) {
                out = in;
                break;
            }
        }
        return out;
    }

    template <Tag T, SeriesViewOrSeries S>
    inline constexpr decltype(auto) operator()(S &&s,
                                               [[maybe_unused]] T t) const {

        result_type_t<S> temp;
        this->operator()(std::forward<S>(s), temp);
        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<S>(s);
        } else {
            this->operator()(std::forward<S>(s), temp);
            return temp;
        }
    }

    template <FrameViewOrFrame F1, Frame F2>
    inline decltype(auto) operator()(F1 const &in, F2 &out) const noexcept {
        auto cols = in.cols();
        auto rows = in.rows();
        using value_type = typename std::decay_t<F2>::value_type;
        out.resize(cols, value_type(rows));

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < cols; ++i) {
            this->operator()(in[i],out[i]);
        }
        return static_cast<F2 &>(out);
    }

    template <Tag T, FrameViewOrFrame F>
    inline constexpr decltype(auto) operator()(F &&s,
                                               [[maybe_unused]] T t) const {

        result_type_t<F> temp;
        this->operator()(std::forward<F>(s), temp);

        if constexpr (std::is_same_v<T, in_place_t>) {
            s = std::move(temp);
            return std::forward<F>(s);
        } else {
            return temp;
        }
    }
};

} // namespace amt::fn

#endif // AMT_FUNCTIONS_TO_HPP
