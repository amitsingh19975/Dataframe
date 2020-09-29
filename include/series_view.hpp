#if !defined(AMT_SERIES_VIEW_HPP)
#define AMT_SERIES_VIEW_HPP

#include <core/concepts.hpp>
#include <slice.hpp>

namespace amt {

template <typename T, bool Const> struct view;

} // namespace amt

namespace amt {

template <bool Const, typename... Ts> struct view<series<Ts...>, Const> {
    static constexpr auto const is_const = Const;

    using view_of = series<Ts...>;
    using name_type = std::string_view;
    using size_type = typename view_of::size_type;
    using value_type = typename view_of::value_type;

    using pointer = std::conditional_t<Const, typename view_of::const_pointer,
                                       typename view_of::pointer>;
    using slice_type = basic_slice<size_type>;

    struct iterator {

        using self_type = iterator;
        using value_type =
            std::conditional_t<Const, const typename view_of::value_type,
                               typename view_of::value_type>;
        using reference =
            std::conditional_t<Const, typename view_of::const_reference,
                               typename view_of::reference>;

        using pointer =
            std::conditional_t<Const, typename view_of::const_pointer,
                               typename view_of::pointer>;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;

        constexpr iterator(pointer ptr, slice_type const &s)
            : m_ptr(ptr), m_sl(s) {}

        [[nodiscard]] inline constexpr self_type
        operator+(difference_type k) const noexcept {
            auto temp = *this;
            temp.m_ptr += m_sl.step() * k;
            return temp;
        }

        [[nodiscard]] inline constexpr self_type
        operator-(difference_type k) const noexcept {
            auto temp = *this;
            temp.m_ptr -= m_sl.step() * k;
            return temp;
        }

        [[nodiscard]] inline constexpr self_type
        operator+=(difference_type k) noexcept {
            m_ptr += m_sl.step() * k;
            return *this;
        }

        [[nodiscard]] inline constexpr self_type
        operator-=(difference_type k) noexcept {
            m_ptr -= m_sl.step() * k;
            return *this;
        }

        [[nodiscard]] inline constexpr self_type operator++() noexcept {
            auto temp = *this;
            m_ptr += m_sl.step();
            return temp;
        }

        [[nodiscard]] inline constexpr self_type &operator++(int) noexcept {
            m_ptr += m_sl.step();
            return *this;
        }

        [[nodiscard]] inline constexpr self_type operator--() noexcept {
            auto temp = *this;
            m_ptr -= m_sl.step();
            return temp;
        }

        [[nodiscard]] inline constexpr self_type &operator--(int) noexcept {
            m_ptr -= m_sl.step();
            return *this;
        }

        [[nodiscard]] inline constexpr reference operator*() noexcept {
            return *m_ptr;
        }

        [[nodiscard]] inline constexpr pointer operator->() noexcept {
            return m_ptr;
        }

        [[nodiscard]] inline constexpr bool
        operator==(self_type const &RHS) const noexcept {
            return m_ptr == RHS.m_ptr;
        }

        [[nodiscard]] inline constexpr bool
        operator!=(self_type const &RHS) const noexcept {
            return !(*this == RHS);
        }

        [[nodiscard]] inline constexpr bool
        operator<(self_type const &RHS) const noexcept {
            return m_ptr < RHS.m_ptr;
        }

        [[nodiscard]] inline constexpr bool
        operator<=(self_type const &RHS) const noexcept {
            return m_ptr <= RHS.m_ptr;
        }

        [[nodiscard]] inline constexpr bool
        operator>(self_type const &RHS) const noexcept {
            return m_ptr > RHS.m_ptr;
        }

        [[nodiscard]] inline constexpr bool
        operator>=(self_type const &RHS) const noexcept {
            return m_ptr >= RHS.m_ptr;
        }

        [[nodiscard]] inline constexpr reference
        operator[](difference_type k) noexcept {
            return *(m_ptr + k * m_sl.step());
        }

      private:
        pointer m_ptr;
        slice_type const &m_sl;
    };

    using const_iterator = typename view<view_of, true>::iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using const_pointer = typename view_of::const_pointer;
    using reference = typename iterator::reference;
    using const_reference = typename view_of::const_reference;

    constexpr view() noexcept = default;

    constexpr view(view const &other) noexcept
        : m_data(other.m_data), m_name(other.m_name), m_slice(other.m_slice),
          m_size(other.m_size) {}

    constexpr view(view &&other) noexcept
        : m_data(std::move(other.m_data)), m_name(other.m_name),
          m_slice(std::move(other.m_slice)), m_size(std::move(other.m_size)) {}

    constexpr view &operator=(view const &other) noexcept = default;

    constexpr view &operator=(view &&other) noexcept = default;

    ~view() = default;

    constexpr view(Series auto &&s, slice_type sl)
        : m_data(s.data())
        , m_name(s.name())
        , m_slice(detail::norm_slice(std::move(sl), s.size()))
        , m_size(s.size() ? m_slice.size() : 0ul) {}

    constexpr view(Series auto &&s) : view(s, slice_type{}) {}

    [[nodiscard]] inline constexpr size_type size() const noexcept {
        return m_size;
    }

    [[nodiscard]] inline constexpr bool empty() const noexcept {
        return m_size == 0;
    }

    [[nodiscard]] inline constexpr reference operator[](size_type k) noexcept {
        return *(m_data + m_slice.first() + k * m_slice.step());
    }

    [[nodiscard]] inline constexpr const_reference
    operator[](size_type k) const noexcept {
        return *(m_data + m_slice.first() + k * m_slice.step());
    }

    [[nodiscard]] inline constexpr reference at(size_type k) {
        if (k >= m_size) {
            throw std::out_of_range("amt::view<series>::at(size_type) : "
                                    "out of bound");
        }
        return *(m_data + m_slice.first() + k * m_slice.step());
    }

    [[nodiscard]] inline constexpr const_reference at(size_type k) const {
        if (k >= m_size) {
            throw std::out_of_range("amt::view<series>::at(size_type) : "
                                    "out of bound");
        }
        return *(m_data + m_slice.first() + k * m_slice.step());
    }

    [[nodiscard]] inline constexpr reference back() {
        return at(m_slice.last());
    }

    [[nodiscard]] inline constexpr const_reference back() const {
        return at(m_slice.last());
    }

    [[nodiscard]] inline constexpr pointer data() noexcept { return m_data; }

    [[nodiscard]] inline constexpr const_pointer data() const noexcept {
        return m_data;
    }

    [[nodiscard]] inline constexpr slice_type slice() const noexcept {
        return m_slice;
    }

    [[nodiscard]] inline constexpr iterator begin() noexcept {
        return iterator(m_data + m_slice.first(), m_slice);
    }

    [[nodiscard]] inline constexpr iterator end() noexcept {
        auto first = m_slice.first();
        auto step = m_slice.step();

        auto sz = first + m_size * step;
        return iterator(m_data + sz, m_slice);
    }

    [[nodiscard]] inline constexpr const_iterator begin() const noexcept {
        return const_iterator(m_data + m_slice.first(), m_slice);
    }

    [[nodiscard]] inline constexpr const_iterator end() const noexcept {
        auto first = m_slice.first();
        auto step = m_slice.step();

        auto sz = first + m_size * step;
        return const_iterator(m_data + sz, m_slice);
    }

    [[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept {
        auto first = m_slice.first();
        auto step = m_slice.step();

        auto sz = first + m_size * step - step;
        auto temp = iterator(m_data + sz, m_slice);
        return std::reverse_iterator<iterator>(temp);
    }

    [[nodiscard]] inline constexpr reverse_iterator rend() noexcept {
        auto first = static_cast<std::ptrdiff_t>(m_slice.first());
        auto step = static_cast<std::ptrdiff_t>(m_slice.step());

        auto sz = (first - step);
        auto temp = iterator(m_data + sz, m_slice);
        return std::reverse_iterator<iterator>(temp);
    }

    [[nodiscard]] inline constexpr const_reverse_iterator
    rbegin() const noexcept {
        auto first = m_slice.first();
        auto step = m_slice.step();

        auto sz = first + m_size * step - step;
        auto temp = iterator(m_data + sz, m_slice);
        return std::reverse_iterator<iterator>(temp);
    }

    [[nodiscard]] inline constexpr const_reverse_iterator
    rend() const noexcept {
        auto first = static_cast<std::ptrdiff_t>(m_slice.first());
        auto step = static_cast<std::ptrdiff_t>(m_slice.step());

        auto sz = (first - step);
        auto temp = iterator(m_data + sz, m_slice);
        return std::reverse_iterator<iterator>(temp);
    }

    friend void swap(view &LHS, view &RHS) noexcept {
        std::swap(LHS.m_data, RHS.m_data);
        std::swap(LHS.m_slice, RHS.m_slice);
        std::swap(LHS.m_size, RHS.m_size);
        std::swap(LHS.m_name, RHS.m_name);
    }

    [[nodiscard]] constexpr name_type name() const noexcept{
        return m_name;
    }

  private:
    pointer m_data;
    name_type m_name;
    slice_type m_slice;
    size_type m_size{};
};

template <Series T, typename S>
view(T const &s, basic_slice<S> const &) -> view<std::decay_t<T>, true>;

template <Series T, typename S>
view(T &s, basic_slice<S> const &) -> view<std::decay_t<T>, false>;

template <Series T> view(T const &s) -> view<std::decay_t<T>, true>;

template <Series T> view(T &s) -> view<std::decay_t<T>, false>;

template <View V>
requires Series<typename V::view_of> std::string type_to_string(V const &s) {
    return s.empty() ? "Empty" : type_to_string(s[0]);
}

} // namespace amt

#endif // AMT_SERIES_VIEW_HPP
