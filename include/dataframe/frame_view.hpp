#if !defined(AMT_DATAFRAME_FRAME_VIEW_HPP)
#define AMT_DATAFRAME_FRAME_VIEW_HPP

#include <dataframe/core/tag.hpp>
#include <dataframe/frame_utils.hpp>
#include <dataframe/series.hpp>
#include <dataframe/slice.hpp>

namespace amt {

template <Series ValueType> struct basic_frame_view<ValueType> {

    using value_type = std::decay_t<ValueType>;
    using box_type = typename value_type::value_type;
    using view_of = basic_frame<box_type>;
    using size_type = typename view_of::size_type;
    using col_view_type = basic_column_view<box_type>;
    using const_col_view_type = basic_column_view<box_type const>;
    using reference = typename view_of::reference;
    using const_reference = typename view_of::const_reference;
    using pointer = typename view_of::pointer;
    using const_pointer = typename view_of::const_pointer;
    using slice_type = basic_slice<size_type>;
    using shape_type = typename view_of::shape_type;
    using series_pointer = ValueType *;
    using row_view_type = basic_row_view<value_type>;
    using const_row_view_type = basic_row_view<value_type const>;

    struct iterator {
        using frame_type = basic_frame_view<ValueType>;
        using value_type = typename frame_type::value_type;
        using size_type = typename frame_type::size_type;
        using col_view_type =
            std::conditional_t<std::is_const_v<ValueType>,
                               typename frame_type::const_col_view_type,
                               typename frame_type::col_view_type>;
        using slice_type = typename frame_type::slice_type;
        using series_pointer = ValueType *;
        using pointer = col_view_type *;
        using reference = col_view_type &;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;

        constexpr iterator(iterator const &other) noexcept = default;
        constexpr iterator(iterator &&other) noexcept = default;
        constexpr iterator &operator=(iterator const &other) noexcept = default;
        constexpr iterator &operator=(iterator &&other) noexcept = default;
        constexpr ~iterator() noexcept = default;

        constexpr iterator(series_pointer ptr, slice_type cs,
                           slice_type rs) noexcept
            : m_ptr(ptr), m_col(cs), m_row(rs) {}

        constexpr iterator operator+(difference_type k) noexcept {
            auto temp = *this;
            temp.m_ptr += k * static_cast<difference_type>(m_col.step());
            return temp;
        }

        constexpr difference_type operator-(iterator const &rhs) noexcept {
            return (m_ptr - rhs.m_ptr);
        }

        constexpr iterator operator-(difference_type k) noexcept {
            auto temp = *this;
            temp.m_ptr += k * static_cast<difference_type>(m_col.step());
            return temp;
        }

        constexpr iterator &operator+=(difference_type k) noexcept {
            auto temp = this->operator+(k);
            swap(temp, *this);
            return *this;
        }

        constexpr iterator &operator-=(difference_type k) noexcept {
            auto temp = this->operator-(k);
            swap(temp, *this);
            return *this;
        }

        constexpr iterator &operator++() noexcept {
            this->operator+=(1);
            return *this;
        }

        constexpr iterator &operator--() noexcept {
            this->operator-=(1);
            return *this;
        }

        constexpr iterator operator++(int) noexcept {
            auto temp = *this;
            temp += 1;
            return temp;
        }

        constexpr iterator operator--(int) noexcept {
            auto temp = *this;
            temp -= 1;
            return temp;
        }

        constexpr reference operator*() noexcept {
            m_view = (*m_ptr)(m_row);
            return m_view;
        }

        constexpr pointer operator->() noexcept {
            m_view = (*m_ptr)(m_row);
            return &m_view;
        }

        constexpr bool operator==(iterator const &rhs) const noexcept {
            return m_ptr == rhs.m_ptr;
        }

        constexpr bool operator!=(iterator const &rhs) const noexcept {
            return !(*this == rhs);
        }

        constexpr bool operator<(iterator const &rhs) const noexcept {
            return m_ptr < rhs.m_ptr;
        }

        constexpr bool operator>(iterator const &rhs) const noexcept {
            return rhs < *this;
        }

        constexpr bool operator<=(iterator const &rhs) const noexcept {
            return (*this < rhs) && (*this == rhs);
        }

        constexpr bool operator>=(iterator const &rhs) const noexcept {
            return (*this > rhs) && (*this == rhs);
        }

        constexpr reference operator[](difference_type k) const noexcept {
            auto step = static_cast<difference_type>(m_col.step());
            m_view = (*(m_ptr + k * step))(m_row);
            return m_view;
        }

        friend void swap(iterator &l, iterator &r) noexcept {
            std::swap(l.m_ptr, r.m_ptr);
            std::swap(l.m_col, r.m_col);
            std::swap(l.m_row, r.m_row);
            std::swap(l.m_view, r.m_view);
        }

      private:
        series_pointer m_ptr;
        slice_type m_col{};
        slice_type m_row{};
        col_view_type m_view{};
    };

    using const_iterator = typename basic_frame_view<ValueType const>::iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr basic_frame_view() noexcept = default;
    constexpr basic_frame_view(basic_frame_view const &other) noexcept =
        default;
    constexpr basic_frame_view(basic_frame_view &&other) noexcept = default;
    constexpr basic_frame_view &
    operator=(basic_frame_view const &other) noexcept = default;
    constexpr basic_frame_view &
    operator=(basic_frame_view &&other) noexcept = default;
    constexpr ~basic_frame_view() noexcept = default;

    constexpr basic_frame_view(series_pointer data, size_type cols,
                               size_type rows, slice_type const &cs,
                               slice_type const &rs)
        : m_data(data), m_col_slice(detail::norm_slice(cs, cols)),
          m_row_slice(detail::norm_slice(rs, rows)), m_cols(cols),
          m_rows(rows) {}

    template <PureFrame FrameType>
    requires(is_frame_same_v<view_of, FrameType>) constexpr basic_frame_view(
        FrameType &&other) {
        if (other.shape() != shape()) {
            throw std::length_error(
                "amt::basic_frame_view(FrameType &&) : shape mismatch");
        }
        std::move(other.begin(), other.end(), begin());
    }

    template <PureFrame FrameType>
    requires(is_frame_same_v<view_of, FrameType>) constexpr basic_frame_view(
        FrameType const &other) {
        if (other.shape() != shape()) {
            throw std::length_error(
                "amt::basic_frame_view(FrameType const&) : shape mismatch");
        }
        std::copy(other.begin(), other.end(), begin());
    }

    constexpr size_type rows() const noexcept {
        return m_rows ? m_row_slice.size() : 0u;
    }
    constexpr size_type cols() const noexcept {
        return m_cols ? m_col_slice.size() : 0u;
    }

    constexpr col_view_type operator[](size_type k) {
        auto pos = m_col_slice.first() + k * m_col_slice.step();
        return m_data[pos](m_row_slice);
    }

    constexpr const_col_view_type operator[](size_type k) const {
        auto pos = m_col_slice.first() + k * m_col_slice.step();
        return m_data[pos](m_row_slice);
    }

    constexpr col_view_type at(size_type k) {
        auto pos = m_col_slice.first() + k * m_col_slice.step();
        auto sz = m_col_slice.last();
        if (pos > sz) {
            throw std::out_of_range("amt::basic_frame_view::at(size_type)");
        }
        return m_data[pos](m_row_slice);
    }

    constexpr const_col_view_type at(size_type k) const {
        auto pos = m_col_slice.first() + k * m_col_slice.step();
        auto sz = m_col_slice.last();
        if (pos > sz) {
            throw std::out_of_range("amt::basic_frame_view::at(size_type)");
        }
        return m_data[pos](m_row_slice);
    }

    constexpr box_type &at(size_type c, size_type r) {
        auto cpos = m_col_slice.first() + c * m_col_slice.step();
        auto rpos = m_row_slice.first() + r * m_row_slice.step();
        auto csz = m_col_slice.last();
        auto rsz = m_row_slice.last();
        if (cpos > csz || rpos > rsz) {
            throw std::out_of_range(
                "amt::basic_frame_view::at(size_type,size_type)");
        }
        return m_data[cpos][rpos];
    }

    constexpr box_type const &at(size_type c, size_type r) const {
        auto cpos = m_col_slice.first() + c * m_col_slice.step();
        auto rpos = m_row_slice.first() + r * m_row_slice.step();
        auto csz = m_col_slice.last();
        auto rsz = m_row_slice.last();
        if (cpos > csz || rpos > rsz) {
            throw std::out_of_range(
                "amt::basic_frame_view::at(size_type,size_type)");
        }
        return m_data[cpos][rpos];
    }

    constexpr box_type &operator()(size_type c, size_type r) {
        auto cpos = m_col_slice.first() + c * m_col_slice.step();
        auto rpos = m_row_slice.first() + r * m_row_slice.step();
        return m_data[cpos][rpos];
    }

    constexpr box_type const &operator()(size_type c, size_type r) const {
        auto cpos = m_col_slice.first() + c * m_col_slice.step();
        auto rpos = m_row_slice.first() + r * m_row_slice.step();
        return m_data[cpos][rpos];
    }

    constexpr bool empty() const noexcept { return m_cols == 0u; }

    constexpr col_view_type back() { return *std::prev(end()); }

    constexpr const_col_view_type back() const { return *std::prev(end()); }

    constexpr iterator begin() noexcept {
        return {m_data + m_col_slice.first(), m_col_slice, m_row_slice};
    }

    constexpr iterator end() noexcept {
        auto last = cols() * m_col_slice.step() + m_col_slice.first();
        return {m_data + last, m_col_slice, m_row_slice};
    }

    constexpr const_iterator begin() const noexcept {
        return {m_data + m_col_slice.first(), m_col_slice, m_row_slice};
    }

    constexpr const_iterator end() const noexcept {
        auto last = cols() * m_col_slice.step() + m_col_slice.first();
        return {m_data + last, m_col_slice, m_row_slice};
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator{end()};
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{begin()};
    }

    constexpr pointer data() noexcept { return m_data + m_col_slice.first(); }

    constexpr const_pointer data() const noexcept {
        return m_data + m_col_slice.first();
    }

    constexpr shape_type shape() const noexcept { return {cols(), rows()}; }

    constexpr row_view_type get_row(size_type k) {
        auto pos = m_row_slice.first() + k * m_row_slice.step();
        return {data(), pos, cols()};
    }

    constexpr const_row_view_type get_row(size_type k) const {
        auto pos = m_row_slice.first() + k * m_row_slice.step();
        return {data(), pos, cols()};
    }

    constexpr col_view_type get_col(size_type k) {
        return this->operator[](k());
    }

    constexpr const_col_view_type get_col(size_type k) const {
        return this->operator[](k());
    }

    constexpr auto operator()(arg::col k) { return get_col(k()); }

    constexpr auto operator()(arg::col k) const { return get_col(k()); }

    constexpr auto operator()(arg::row k) { return get_row(k()); }

    constexpr auto operator()(arg::row k) const { return get_row(k()); }

    constexpr auto operator()(size_type k, tag::col_t) { return get_col(k); }

    constexpr auto operator()(size_type k, tag::col_t) const {
        return get_col(k);
    }

    constexpr auto operator()(size_type k, tag::row_t) { return get_row(k); }

    constexpr auto operator()(size_type k, tag::row_t) const {
        return get_row(k);
    }

    constexpr operator basic_frame_view<ValueType const>() const noexcept {
        return {m_data, m_col_slice, m_row_slice, m_cols, m_rows};
    }

    constexpr basic_frame_view operator()(slice_type cs,
                                          slice_type rs = {}) const noexcept {
        return {m_data, m_col_slice(cs), m_row_slice(rs), m_cols, m_rows};
    }

    constexpr basic_frame_view<ValueType const>
    operator()(slice_type cs, slice_type rs = {}) noexcept {
        return {m_data, m_col_slice(cs), m_row_slice(rs), m_cols, m_rows};
    }

  private:
    series_pointer m_data;
    slice_type m_col_slice;
    slice_type m_row_slice;
    size_type m_cols{};
    size_type m_rows{};
};

using frame_view = basic_frame_view<series>;

} // namespace amt

#endif // AMT_DATAFRAME_FRAME_VIEW_HPP
