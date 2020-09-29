#if !defined(AMT_FUNCTIONS_TAIL_HPP)
#define AMT_FUNCTIONS_TAIL_HPP

#include <algorithm>
#include <core/concepts.hpp>
#include <unordered_set>

namespace amt::fn {

struct tail_fn {

    template <SeriesViewOrSeries S1, Series S2>
    inline decltype(auto) operator()(S1 const &in, S2 &out,
                                     std::size_t max = 5ul) const noexcept {
                                        
        auto sz = in.size();
        max = std::min(sz, max);
        
        out.resize(max);
        out.name() = in.name();

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < max; ++i) {
            out[max - i - 1ul] = in[sz - i - 1ul];
        }

        return static_cast<S2 &>(out);
    }

    template <SeriesViewOrSeries S>
    inline decltype(auto) operator()(S &&s,
                                     std::size_t max = 5ul) const noexcept {
        result_type_t<S> temp;
        this->operator()(std::forward<S>(s), temp, max);
        return temp;
    }

    template <FrameViewOrFrame F1, Frame F2>
    inline decltype(auto) operator()(F1 const &in, F2 &out,
                                     std::size_t max = 5ul) const noexcept {
        auto cols = in.cols();
        auto rows = in.rows();
        max = std::min(rows, max);

        out.resize(cols, max);

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < cols; ++i) {
            this->operator()(in[i], out[i], max);
        }

        return static_cast<F2 &>(out);
    }

    template <FrameViewOrFrame F>
    inline decltype(auto) operator()(F &&s,
                                     std::size_t max = 5ul) const noexcept {

        result_type_t<F> temp;
        this->operator()(std::forward<F>(s), temp, max);
        return temp;
    }
};

} // namespace amt::fn

#endif // AMT_FUNCTIONS_TAIL_HPP
