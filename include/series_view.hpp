#if !defined(AMT_DATAFRAME_SERIES_VIEW_HPP)
#define AMT_DATAFRAME_SERIES_VIEW_HPP

#include <core/tag.hpp>
#include <series_utils.hpp>
#include <slice.hpp>
#include <traits/series_traits.hpp>

namespace amt {

template <Box ValueType> struct basic_series_view<ValueType, tag::col_t> {

    using view_of = basic_series<ValueType>;
    using value_type = ValueType;
    using name_type = std::string *;
    using size_type = typename view_of::size_type;
    using reference = typename view_of::reference;
    using const_reference = typename view_of::const_reference;
    using pointer = typename view_of::pointer;
    using const_pointer = typename view_of::const_pointer;
    using slice_type = basic_slice<size_type>;
    using axis_tag = tag::col_t;

    struct iterator {
        using value_type = ValueType;
        using slice_type = basic_series_view::slice_type;
        using reference = ValueType &;
        using pointer = ValueType *;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;

        constexpr iterator(iterator const &other) noexcept = default;
        constexpr iterator(iterator &&other) noexcept = default;
        constexpr iterator &operator=(iterator const &other) noexcept = default;
        constexpr iterator &operator=(iterator &&other) noexcept = default;
        constexpr ~iterator() noexcept = default;

        constexpr iterator(pointer ptr, slice_type const &s) noexcept
            : m_ptr(ptr), m_slice(s) {}

        constexpr iterator operator+(difference_type k) noexcept {
            auto temp = *this;
            temp.m_ptr += k * static_cast<difference_type>(m_slice.step());
            return temp;
        }

        constexpr difference_type operator-(iterator const &rhs) noexcept {
            return (m_ptr - rhs.m_ptr);
        }

        constexpr iterator operator-(difference_type k) noexcept {
            auto temp = *this;
            temp.m_ptr -= k * static_cast<difference_type>(m_slice.step());
            return temp;
        }

        constexpr iterator operator+=(difference_type k) noexcept {
            m_ptr += k * static_cast<difference_type>(m_slice.step());
            return *this;
        }

        constexpr iterator operator-=(difference_type k) noexcept {
            m_ptr -= k * static_cast<difference_type>(m_slice.step());
            return *this;
        }

        constexpr iterator operator++() noexcept {
            m_ptr += static_cast<difference_type>(m_slice.step());
            return *this;
        }

        constexpr iterator operator--() noexcept {
            m_ptr -= static_cast<difference_type>(m_slice.step());
            return *this;
        }

        constexpr iterator operator++(int) noexcept {
            auto temp = *this;
            temp.m_ptr += static_cast<difference_type>(m_slice.step());
            return temp;
        }

        constexpr iterator operator--(int) noexcept {
            auto temp = *this;
            temp.m_ptr -= static_cast<difference_type>(m_slice.step());
            return temp;
        }

        constexpr reference operator*() noexcept { return *m_ptr; }

        constexpr pointer operator->() noexcept { return m_ptr; }

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
            auto pos = k * static_cast<difference_type>(m_slice.step());
            return *(m_ptr + pos);
        }

      private:
        pointer m_ptr;
        slice_type m_slice;
    };

    using const_iterator =
        typename basic_series_view<value_type const, tag::col_t>::iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr basic_series_view() noexcept = default;
    constexpr basic_series_view(basic_series_view const &other) noexcept =
        default;
    constexpr basic_series_view(basic_series_view &&other) noexcept = default;
    constexpr basic_series_view &
    operator=(basic_series_view const &other) noexcept = default;
    constexpr basic_series_view &
    operator=(basic_series_view &&other) noexcept = default;
    constexpr ~basic_series_view() noexcept = default;

    constexpr basic_series_view(pointer data, name_type name, size_type size,
                                DType auto d, slice_type const &s)
        : m_data(data), m_slice(detail::norm_slice(s, size)), m_name(name),
          m_dtype(get<value_type>(d)), m_size(size ? m_slice.size() : 0u) {}

    template <PureSeries SeriesType>
    requires(is_series_same_v<view_of, SeriesType>) constexpr basic_series_view(
        SeriesType &&other) {
        if (other.size() != size()) {
            throw std::length_error(
                "amt::basic_series_view(SeriesType &&) : size mismatch");
        }
        parallel_move(other.begin(), other.end(), begin());
    }

    template <PureSeries SeriesType>
    requires(is_series_same_v<view_of, SeriesType>) constexpr basic_series_view(
        SeriesType const &other) {
        if (other.size() != size()) {
            throw std::length_error(
                "amt::basic_series_view(SeriesType const&) : size mismatch");
        }
        parallel_copy(other.begin(), other.end(), begin());
    }

    constexpr size_type size() const noexcept { return m_size; }

    constexpr reference operator[](size_type k) {
        auto pos = m_slice.first() + k * m_slice.step();
        return m_data[pos];
    }

    constexpr const_reference operator[](size_type k) const {
        auto pos = m_slice.first() + k * m_slice.step();
        return m_data[pos];
    }

    constexpr reference at(size_type k) {
        auto pos = m_slice.first() + k * m_slice.step();
        auto sz = m_slice.last();
        if (pos > sz) {
            throw std::out_of_range("amt::basic_series_view::at(size_type)");
        }
        return m_data[pos];
    }

    constexpr const_reference at(size_type k) const {
        auto pos = m_slice.first() + k * m_slice.step();
        auto sz = m_slice.last();
        if (pos > sz) {
            throw std::out_of_range("amt::basic_series_view::at(size_type)");
        }
        return m_data[pos];
    }

    constexpr bool empty() const noexcept { return m_size == 0u; }

    constexpr reference back() { return *std::prev(end()); }

    constexpr const_reference back() const { return *std::prev(end()); }

    constexpr std::string const &name() const {
        if (m_name == nullptr) {
            throw std::runtime_error(ERR_CSTR(
                "amt::basic_series_view::name() : dereferencing null pointer"));
        }
        return *m_name;
    }

    constexpr std::string &name() {
        if (m_name == nullptr) {
            throw std::runtime_error(ERR_CSTR(
                "amt::basic_series_view::name() : dereferencing null pointer"));
        }
        return *m_name;
    }

    constexpr void name(std::string_view name) {
        if (m_name == nullptr) {
            throw std::runtime_error(
                ERR_CSTR("amt::basic_series_view::name(std::string_view) : "
                         "copying string to to null pointer"));
        }
        (*m_name) = std::string(name);
    }

    constexpr iterator begin() noexcept {
        return {m_data + m_slice.first(), m_slice};
    }

    constexpr iterator end() noexcept {
        auto last = m_size * m_slice.step() + m_slice.first();
        return {m_data + last, m_slice};
    }

    constexpr const_iterator begin() const noexcept {
        return {m_data + m_slice.first(), m_slice};
    }

    constexpr const_iterator end() const noexcept {
        auto last = m_size * m_slice.step() + m_slice.first();
        return {m_data + last, m_slice};
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

    constexpr pointer data() noexcept { return m_data + m_slice.first(); }

    constexpr const_pointer data() const noexcept {
        return m_data + m_slice.first();
    }

    constexpr
    operator basic_series_view<ValueType const, tag::col_t>() const noexcept {
        return {m_data, m_name, m_size, std::move(m_slice)};
    }

    template <PureSeries SeriesType> constexpr operator SeriesType() const {
        SeriesType temp(m_size);
        parallel_transform(begin(), end(), temp.begin(),
                           [](auto &&val) { return val; });

        if (m_name)
            temp.name(*m_name);
    }

    constexpr ::amt::dtype<> dtype() const noexcept { return m_dtype; }

  private:
    pointer m_data;
    slice_type m_slice;
    name_type m_name{nullptr};
    ::amt::dtype<> m_dtype{};
    size_type m_size{};
};

template <Series SeriesType> struct basic_series_view<SeriesType, tag::row_t> {

    using view_of = SeriesType;
    using name_type = std::string_view;
    using value_type = typename SeriesType::value_type;
    using size_type = typename view_of::size_type;
    using reference = std::conditional_t< std::is_const_v<view_of>, value_type const&, value_type& >;
    using const_reference = typename view_of::const_reference;
    using pointer = std::conditional_t< std::is_const_v<view_of>, value_type const*, value_type* >;
    using const_pointer = typename view_of::const_pointer;
    using series_pointer_t = std::add_pointer_t<view_of>;
    using axis_tag = tag::row_t;

    struct iterator {
        using value_type = typename view_of::value_type;
        using size_type = typename view_of::size_type;
        using reference = std::conditional_t< std::is_const_v<view_of>, value_type const&, value_type& >;
        using pointer = std::conditional_t< std::is_const_v<view_of>, value_type const*, value_type* >;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;

        constexpr iterator(iterator const &other) noexcept = default;
        constexpr iterator(iterator &&other) noexcept = default;
        constexpr iterator &operator=(iterator const &other) noexcept = default;
        constexpr iterator &operator=(iterator &&other) noexcept = default;
        constexpr ~iterator() noexcept = default;

        constexpr iterator(series_pointer_t ptr, size_type row) noexcept
            : m_ptr(ptr), m_row(row) {}

        constexpr iterator operator+(difference_type k) noexcept {
            auto temp = *this;
            temp.m_ptr += k;
            return temp;
        }

        constexpr difference_type operator-(iterator const &rhs) noexcept {
            return (m_ptr - rhs.m_ptr);
        }

        constexpr iterator operator-(difference_type k) noexcept {
            auto temp = *this;
            temp.m_ptr -= k;
            return temp;
        }

        constexpr iterator operator+=(difference_type k) noexcept {
            m_ptr += k;
            return *this;
        }

        constexpr iterator operator-=(difference_type k) noexcept {
            m_ptr -= k;
            return *this;
        }

        constexpr iterator operator++() noexcept {
            ++m_ptr;
            return *this;
        }

        constexpr iterator operator--() noexcept {
            --m_ptr;
            return *this;
        }

        constexpr iterator operator++(int) noexcept {
            auto temp = *this;
            ++temp.m_ptr;
            return temp;
        }

        constexpr iterator operator--(int) noexcept {
            auto temp = *this;
            --temp.m_ptr;
            return temp;
        }

        constexpr reference operator*() noexcept { return (*m_ptr)[m_row]; }

        constexpr pointer operator->() noexcept { return &((*m_ptr)[m_row]); }

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
            return (*(m_ptr + k))[m_row];
        }

      private:
        series_pointer_t m_ptr;
        size_type m_row{};
    };

    using const_iterator =
        typename basic_series_view<SeriesType const, tag::row_t>::iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr basic_series_view() noexcept = default;
    constexpr basic_series_view(basic_series_view const &other) noexcept =
        default;
    constexpr basic_series_view(basic_series_view &&other) noexcept = default;
    constexpr basic_series_view &
    operator=(basic_series_view const &other) noexcept = default;
    constexpr basic_series_view &
    operator=(basic_series_view &&other) noexcept = default;
    constexpr ~basic_series_view() noexcept = default;

    constexpr basic_series_view(series_pointer_t data, size_type row, size_type col, std::string_view name = "")
        : m_data(data), m_name(name), m_row(row), m_size(col) {}

    template <PureSeries OtherSeriesType>
    requires(is_series_same_v<view_of, OtherSeriesType>) constexpr basic_series_view(
        OtherSeriesType &&other) {
        if (other.size() != size()) {
            throw std::length_error(
                "amt::basic_series_view(OtherSeriesType &&) : size mismatch");
        }
        parallel_move(other.begin(), other.end(), begin());
    }

    template <PureSeries OtherSeriesType>
    requires(is_series_same_v<view_of, OtherSeriesType>) constexpr basic_series_view(
        OtherSeriesType const &other) {
        if (other.size() != size()) {
            throw std::length_error(
                "amt::basic_series_view(OtherSeriesType const&) : size mismatch");
        }
        parallel_copy(other.begin(), other.end(), begin());
    }

    constexpr size_type size() const noexcept { return m_size; }

    constexpr reference operator[](size_type k) { return m_data[k][m_row]; }

    constexpr const_reference operator[](size_type k) const {
        return m_data[k][m_row];
    }

    constexpr reference at(size_type k) {
        if (k >= m_size) {
            throw std::out_of_range("amt::basic_series_view::at(size_type)");
        }
        return m_data[k][m_row];
    }

    constexpr const_reference at(size_type k) const {
        if (k >= m_size) {
            throw std::out_of_range("amt::basic_series_view::at(size_type)");
        }
        return m_data[k][m_row];
    }
    
    constexpr bool empty() const noexcept { return m_size == 0u; }

    constexpr reference back() { return *std::prev(end()); }

    constexpr const_reference back() const { return *std::prev(end()); }

    constexpr iterator begin() noexcept { return {m_data, m_row}; }

    constexpr iterator end() noexcept {
        return {m_data + m_size, m_row};
    }

    constexpr const_iterator begin() const noexcept {
        return {m_data, m_row};
    }

    constexpr const_iterator end() const noexcept {
        return {m_data + m_size, m_row};
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

    constexpr pointer data() noexcept { return m_data; }

    constexpr const_pointer data() const noexcept { return m_data; }

    constexpr
    operator basic_series_view<SeriesType const, tag::row_t>() const noexcept {
        return {m_data, m_row};
    }

    template <PureSeries OtherSeriesType> constexpr operator OtherSeriesType() const {
        SeriesType temp(m_size);
        parallel_transform(begin(), end(), temp.begin(),
                           [](auto &&val) { return val; });
    }

    constexpr name_type name() const noexcept{
        return m_name;
    }

    constexpr name_type name() noexcept{
        return m_name;
    }

    constexpr void name(std::string_view name) {
        m_name = name;
    }

    constexpr ::amt::dtype<> dtype() const noexcept { return {0u}; }

  private:
    series_pointer_t m_data;
    name_type m_name{};
    size_type m_row{};
    size_type m_size{};
};

template<typename ValueType>
using basic_column_view = basic_series_view<ValueType, tag::col_t>;

template<typename ValueType>
using basic_row_view = basic_series_view<ValueType, tag::row_t>;

} // namespace amt

#endif // AMT_DATAFRAME_SERIES_VIEW_HPP
