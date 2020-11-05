#if !defined(AMT_SLICE_HPP)
#define AMT_SLICE_HPP

#include <core/utils.hpp>
#include <cstddef>
#include <limits>
#include <traits/slice_traits.hpp>

namespace amt {

struct end_t {
    template <typename U>
    requires(std::is_integral_v<U> &&
             !std::numeric_limits<U>::is_signed) constexpr
    operator U() const noexcept {
        return std::numeric_limits<U>::max();
    }
};

inline constexpr auto end = end_t{};

namespace arg {

struct first {
    constexpr first() noexcept = default;
    constexpr first(first const &other) noexcept = default;
    constexpr first(first &&other) noexcept = default;
    constexpr first &operator=(first const &other) noexcept = default;
    constexpr first &operator=(first &&other) noexcept = default;
    constexpr ~first() noexcept = default;
    constexpr first(std::size_t d) noexcept : data(d) {}
    std::size_t data{0};
};

struct last {
    constexpr last() noexcept = default;
    constexpr last(last const &other) noexcept = default;
    constexpr last(last &&other) noexcept = default;
    constexpr last &operator=(last const &other) noexcept = default;
    constexpr last &operator=(last &&other) noexcept = default;
    constexpr ~last() noexcept = default;
    constexpr last(std::size_t d) noexcept : data(d) {}
    std::size_t data{end};
};

struct step {
    constexpr step() noexcept = default;
    constexpr step(step const &other) noexcept = default;
    constexpr step(step &&other) noexcept = default;
    constexpr step &operator=(step const &other) noexcept = default;
    constexpr step &operator=(step &&other) noexcept = default;
    constexpr ~step() noexcept = default;
    constexpr step(std::size_t d) noexcept : data(d) {}
    std::size_t data{1};
};

} // namespace arg

namespace literal {

constexpr arg::first operator""_f(unsigned long long int val) noexcept {
    return {val};
}

constexpr arg::last operator""_l(unsigned long long int val) noexcept {
    return {val};
}

constexpr arg::step operator""_s(unsigned long long int val) noexcept {
    return {val};
}

} // namespace literal

template <typename T> struct basic_slice {

    using value_type = T;
    using base_type = std::tuple<value_type, value_type, value_type>;
    using reference = T &;
    using const_reference = T const &;
    using pointer = T *;
    using const_pointer = T const *;
    using size_type = std::size_t;

    template <typename U> friend struct basic_slice;

    static_assert(!std::numeric_limits<value_type>::is_signed,
                  "amt::basic_slice<T> : "
                  "T should be unsigned type");

    constexpr basic_slice() noexcept = default;
    constexpr basic_slice(basic_slice const &other) noexcept = default;
    constexpr basic_slice(basic_slice &&other) noexcept = default;
    constexpr basic_slice &
    operator=(basic_slice const &other) noexcept = default;
    constexpr basic_slice &operator=(basic_slice &&other) noexcept = default;
    constexpr ~basic_slice() noexcept = default;

    template <typename U>
    constexpr basic_slice(basic_slice<U> other) noexcept
        : m_data(std::move(other.m_data)) {}

    template <typename Arg0, typename... Args>
    requires(!Slice<Args> && ... &&
             !Slice<Arg0>) constexpr basic_slice(Arg0 &&arg0, Args &&... arg) {

        static_assert(sizeof...(Args) <= 2,
                      "amt::basic_slice<T>(Arg0&&, Args&&...) : "
                      "arity should be equal to 3 or less");

        auto tp = std::forward_as_tuple(std::forward<Arg0>(arg0),
                                        std::forward<Args>(arg)...);

        tuple_for(tp, [this]<typename U, typename Index>(U const &el, Index) {
            constexpr auto idx = Index::value;
            if constexpr (std::is_same_v<U, arg::first>) {
                std::get<0>(m_data) = static_cast<value_type>(el.data);
            } else if constexpr (std::is_same_v<U, arg::last>) {
                std::get<1>(m_data) = static_cast<value_type>(el.data);
            } else if constexpr (std::is_same_v<U, arg::step>) {
                std::get<2>(m_data) = static_cast<value_type>(el.data);
            } else {
                std::get<idx>(m_data) = static_cast<value_type>(el);
            }
        });

        if (step() == 0) {
            throw std::runtime_error("amt::basic_slice<T>(Args&&...) : "
                                     "stides or steps can not be zero");
        }
    }

    [[nodiscard]] inline constexpr reference first() noexcept {
        return std::get<0>(m_data);
    }

    [[nodiscard]] inline constexpr const_reference first() const noexcept {
        return std::get<0>(m_data);
    }

    [[nodiscard]] inline constexpr reference last() noexcept {
        return std::get<1>(m_data);
    }

    [[nodiscard]] inline constexpr const_reference last() const noexcept {
        return std::get<1>(m_data);
    }

    [[nodiscard]] inline constexpr reference step() noexcept {
        return std::get<2>(m_data);
    }

    [[nodiscard]] inline constexpr const_reference step() const noexcept {
        return std::get<2>(m_data);
    }

    [[nodiscard]] inline constexpr size_type size() const noexcept {
        return ((last() - first()) / step()) + 1ul;
    }

    constexpr basic_slice operator()(basic_slice const& rhs) const noexcept{
        return {
            rhs.first() * step() + first(),
            rhs.last() * step() + first(),
            rhs.step() * step(),
        };
    }

    friend std::ostream &operator<<(std::ostream &os, basic_slice const &s) {
        return os << "[ first: " << s.first() << ", last: " << s.last()
                  << ", step: " << s.step() << " ]";
    }

  private:
    base_type m_data{value_type{}, end, value_type{1}};
};

using slice = basic_slice<std::size_t>;

} // namespace amt

namespace amt::detail {

template <typename T>
[[nodiscard]] inline constexpr basic_slice<T>
norm_slice(basic_slice<T> const &s, std::size_t size) {

    if (size == 0ul) {
        return {0ul, 0ul, 1ul};
    }

    size -= 1ul;
    auto const fst = s.first() > size ? size : s.first();
    auto const lst = s.last() > size ? size : s.last();
    if (fst > lst) {
        throw std::runtime_error(
            "amt::detail::norma_slice(basic_slice<T> const&, std::size_t) : "
            "last < first");
    }

    return {fst, lst, s.step()};
}

} // namespace amt::detail

#endif // AMT_SLICE_HPP
