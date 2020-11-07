#if !defined(AMT_DATAFRAME_FUNCTIONS_SHUFFLE_HPP)
#define AMT_DATAFRAME_FUNCTIONS_SHUFFLE_HPP

#include <dataframe/traits/series_traits.hpp>
#include <random>

namespace amt {

namespace fn {

struct shuffle_t {
    template <Series SeriesType>
    SeriesType operator()(SeriesType const &s, unsigned seed = 42u,
                          std::size_t iter = 100u) const {
        if (s.empty()) {
            return s;
        }
        using size_type = typename SeriesType::size_type;
        auto temp = s;
        std::default_random_engine gen(seed);
        std::uniform_int_distribution<size_type> dis(0u, s.size() - 1);
        while (iter--) {
            auto p1 = dis(gen);
            auto p2 = dis(gen);
            if (p1 == p2) {
                ++iter;
                continue;
            }
            std::swap(temp[p1], temp[p2]);
        }
        return temp;
    }

    template <Series SeriesType>
    SeriesType &operator()(SeriesType &s, tag::inplace_t, unsigned seed = 42u,
                           std::size_t iter = 100u) const {
        if (s.empty()) {
            return s;
        }
        using size_type = typename SeriesType::size_type;
        std::default_random_engine gen(seed);
        std::uniform_int_distribution<size_type> dis(0u, s.size() - 1);
        while (iter--) {
            auto p1 = dis(gen);
            auto p2 = dis(gen);
            if (p1 == p2) {
                ++iter;
                continue;
            }
            std::swap(s[p1], s[p2]);
        }
        return s;
    }

    template <Frame FrameType>
    FrameType operator()(FrameType const &f, unsigned seed = 42u,
                         std::size_t iter = 100u) const {
        if (f.empty()) {
            return f;
        }
        using size_type = typename FrameType::size_type;
        auto temp = f;
        std::default_random_engine gen(seed);
        std::uniform_int_distribution<size_type> dis(0u, f.rows() - 1);
        while (iter--) {
            auto p1 = dis(gen);
            auto p2 = dis(gen);
            if (p1 == p2) {
                ++iter;
                continue;
            }
            for (auto i = 0u; i < f.cols(); ++i) {
                std::swap(temp[i][p1], temp[i][p2]);
            }
        }
        return temp;
    }

    template <Frame FrameType>
    FrameType &operator()(FrameType &f, tag::inplace_t, unsigned seed = 42u,
                          std::size_t iter = 100u) const {
        if (f.empty()) {
            return f;
        }
        using size_type = typename FrameType::size_type;
        std::default_random_engine gen(seed);
        std::uniform_int_distribution<size_type> dis(0u, f.rows() - 1);
        while (iter--) {
            auto p1 = dis(gen);
            auto p2 = dis(gen);
            if (p1 == p2) {
                ++iter;
                continue;
            }
            for (auto i = 0u; i < f.cols(); ++i) {
                std::swap(f[i][p1], f[i][p2]);
            }
        }
        return f;
    }
};

} // namespace fn

inline static constexpr auto shuffle = fn::shuffle_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_SHUFFLE_HPP
