#if !defined(AMT_SLICE_HPP)
#define AMT_SLICE_HPP

#include <boost/mp11/mpl_tuple.hpp>
#include <core/concepts.hpp>
#include <cstddef>
#include <limits>

namespace amt {

struct slice_first {
    constexpr slice_first() noexcept = default;
    constexpr slice_first(slice_first const& other) noexcept = default;
    constexpr slice_first(slice_first && other) noexcept = default;
    constexpr slice_first& operator=(slice_first const& other) noexcept = default;
    constexpr slice_first& operator=(slice_first && other) noexcept = default;
    ~slice_first() = default;
    constexpr slice_first(std::size_t d) noexcept
        : data(d)
    {}
    std::size_t data{0};
};

struct slice_last {
    constexpr slice_last() noexcept = default;
    constexpr slice_last(slice_last const& other) noexcept = default;
    constexpr slice_last(slice_last && other) noexcept = default;
    constexpr slice_last& operator=(slice_last const& other) noexcept = default;
    constexpr slice_last& operator=(slice_last && other) noexcept = default;
    ~slice_last() = default;
    constexpr slice_last(std::size_t d) noexcept
        : data(d)
    {}
    std::size_t data{std::numeric_limits<std::size_t>::max()};
};

struct slice_stride {
    constexpr slice_stride() noexcept = default;
    constexpr slice_stride(slice_stride const& other) noexcept = default;
    constexpr slice_stride(slice_stride && other) noexcept = default;
    constexpr slice_stride& operator=(slice_stride const& other) noexcept = default;
    constexpr slice_stride& operator=(slice_stride && other) noexcept = default;
    ~slice_stride() = default;
    constexpr slice_stride(std::size_t d) noexcept
        : data(d)
    {}
    std::size_t data{1};
};

template <typename T> struct basic_slice {

    using value_type = T;
    using reference = T &;
    using const_reference = T const &;
    using pointer = T *;
    using const_pointer = T const *;
    using size_type = std::size_t;

    static_assert(!std::numeric_limits<value_type>::is_signed,
                  "amt::basic_slice<T> : "
                  "T should be unsigned type");

    static constexpr auto const end = std::numeric_limits<value_type>::max();

    constexpr basic_slice() noexcept = default;
    constexpr basic_slice(basic_slice const &other) noexcept = default;
    constexpr basic_slice(basic_slice &&other) noexcept = default;
    constexpr basic_slice &
    operator=(basic_slice const &other) noexcept = default;
    constexpr basic_slice &operator=(basic_slice &&other) noexcept = default;
    ~basic_slice() = default;

    template <typename U>
    constexpr basic_slice(basic_slice<U> other) noexcept
        : m_first(std::move(static_cast<value_type>(other.m_first))),
          m_last(std::move(static_cast<value_type>(other.m_last))),
          m_stride(std::move(static_cast<value_type>(other.m_stride))) {}

    template <typename... Args>
    requires is_proper_slice_args_v<Args...> constexpr basic_slice(
        Args &&... args) {

        static_assert(sizeof...(Args) <= 3,
                      "amt::basic_slice<T>(Args&&...) : "
                      "arity should be equal to 3 or less");

        auto tp = std::forward_as_tuple(args...);

        boost::mp11::mp_for_each<boost::mp11::mp_iota_c<sizeof...(Args)>>(
            [&, this](auto I) {
                constexpr auto const idx = decltype(I)::value;
                auto &val = std::get<idx>(tp);
                using val_type = std::decay_t<decltype(val)>;

                if constexpr (std::is_same_v<val_type, slice_first>) {
                    this->m_first = val.data;
                } else if constexpr (std::is_same_v<val_type, slice_last>) {
                    this->m_last = val.data;
                } else if constexpr (std::is_same_v<val_type, slice_stride>) {
                    this->m_stride = val.data;
                } else {
                    if constexpr (idx == 0ul) {
                        m_first = static_cast<value_type>(val);
                    } else if constexpr (idx == 1ul) {
                        m_last = static_cast<value_type>(val);
                    } else {
                        m_stride = static_cast<value_type>(val);
                    }
                }
            });

        if (m_stride == 0) {
            throw std::runtime_error("amt::basic_slice<T>(Args&&...) : "
                                     "stides or steps can not be zero");
        }
    }

    [[nodiscard]] inline constexpr reference first() noexcept {
        return m_first;
    }

    [[nodiscard]] inline constexpr const_reference first() const noexcept {
        return m_first;
    }

    [[nodiscard]] inline constexpr reference last() noexcept { return m_last; }

    [[nodiscard]] inline constexpr const_reference last() const noexcept {
        return m_last;
    }

    [[nodiscard]] inline constexpr reference step() noexcept {
        return m_stride;
    }

    [[nodiscard]] inline constexpr const_reference step() const noexcept {
        return m_stride;
    }

    [[nodiscard]] inline constexpr size_type size() const noexcept {
        return ((m_last - m_first) / m_stride) + 1ul;
    }

  private:
    value_type m_first{};
    value_type m_last{end};
    value_type m_stride{1};
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
