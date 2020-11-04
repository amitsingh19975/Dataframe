#if !defined(AMT_DATAFRAME_SERIES_HPP)
#define AMT_DATAFRAME_SERIES_HPP

#include <box.hpp>
#include <series_view.hpp>
#include <vector>
#include <functions/filter.hpp>

namespace amt {

template <typename ValueType> struct basic_series {
    using value_type = ValueType;
    using name_type = std::string;
    using base_type = std::vector<value_type>;
    using size_type = typename base_type::size_type;
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using reverse_iterator = typename base_type::reverse_iterator;
    using const_reverse_iterator = typename base_type::const_reverse_iterator;
    using reference = typename base_type::reference;
    using const_reference = typename base_type::const_reference;
    using pointer = typename base_type::pointer;
    using const_pointer = typename base_type::const_pointer;
    using view_type = basic_column_view<value_type>;
    using const_view_type = basic_column_view<value_type const>;
    using slice_type = basic_slice<size_type>;

    static_assert(is_box_v<ValueType>,
                  "amt::basic_series : ValueType must be basic_box or "
                  "specialize box traits for custom types");

    constexpr basic_series() = default;
    constexpr basic_series(basic_series const &other) = default;
    constexpr basic_series(basic_series &&other) = default;
    constexpr basic_series &operator=(basic_series const &other) = default;
    constexpr basic_series &operator=(basic_series &&other) = default;
    constexpr ~basic_series() = default;

    // basic_series(name,size)
    constexpr basic_series(std::string_view name, size_type size)
        : m_data(size), m_name(name) {}

    // basic_series(name,size,dtype)
    constexpr basic_series(std::string_view name, size_type size,
                           DType auto dtype)
        : m_data(size, value_type(dtype)), m_name(name),
          m_dtype(get<value_type>(dtype)) {}

    // basic_series(size)
    constexpr basic_series(size_type size) : m_data(size) {}

    // basic_series(size,dtype)
    constexpr basic_series(size_type size, DType auto dtype)
        : m_data(size, value_type(dtype)), m_dtype(get<value_type>(dtype)) {}

    // basic_series(name,dtype)
    constexpr basic_series(std::string_view name, DType auto dtype)
        : m_name(name), m_dtype(get<value_type>(dtype)) {}

    // basic_series(name)
    constexpr basic_series(std::string_view name) : m_name(name) {}

    // basic_series(name, size, data, dtype)
    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             !DType<U>) constexpr basic_series(std::string_view name,
                                               size_type size, U u,
                                               DType auto dtype)
        : m_data(size, value_type(std::move(u), dtype)), m_name(name),
          m_dtype(get<value_type>(dtype)) {}

    // basic_series(name, size, data)
    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             !DType<U>) constexpr basic_series(std::string_view name,
                                               size_type size, U u)
        : m_data(size, value_type(std::move(u))), m_name(name) {}

    // basic_series(size, data, dtype)
    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             !DType<U>) constexpr basic_series(size_type size, U u,
                                               DType auto dtype)
        : m_data(size, value_type(std::move(u), dtype)),
          m_dtype(get<value_type>(dtype)) {}

    // basic_series(size, data)
    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             !DType<U>) constexpr basic_series(size_type size, U u)
        : m_data(size, value_type(std::move(u))) {}

    // basic_series(name, initilizer_list, dtype)
    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             !DType<U>) constexpr basic_series(std::string_view name,
                                               std::initializer_list<U> li,
                                               DType auto dtype)
        : m_data(li.size()), m_name(name), m_dtype(get<value_type>(dtype)) {
        auto i = 0u;
        for (auto &el : li) {
            if constexpr (is_box_v<U>) {
                el.dtype(dtype);
                m_data[i++] = std::move(el);
            } else {
                auto temp = value_type(std::move(el), dtype);
                m_data[i++] = std::move(temp);
            }
        }
    }

    // basic_series(name, initilizer_list)
    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             !DType<U>) constexpr basic_series(std::string_view name,
                                               std::initializer_list<U> li)
        : m_data(li.size()), m_name(name) {
        auto i = 0u;
        for (auto &el : li) {
            auto temp = value_type(std::move(el));
            m_data[i++] = std::move(temp);
        }
    }

    // basic_series(initilizer_list, dtype)
    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             !DType<U>) constexpr basic_series(std::initializer_list<U> li,
                                               DType auto dtype)
        : m_data(li.size()), m_dtype(get<value_type>(dtype)) {
        auto i = 0u;
        for (auto &el : li) {
            if constexpr (is_box_v<U>) {
                el.dtype(dtype);
                m_data[i++] = std::move(el);
            } else {
                auto temp = value_type(std::move(el), dtype);
                m_data[i++] = std::move(temp);
            }
        }
    }

    // basic_series(initilizer_list)
    template <typename U>
    requires(std::is_constructible_v<value_type, U> &&
             !DType<U>) constexpr basic_series(std::initializer_list<U> li)
        : m_data(li.size()) {
        auto i = 0u;
        for (auto &el : li) {
            auto temp = value_type(std::move(el));
            m_data[i++] = std::move(temp);
        }
    }

    constexpr basic_series(base_type li) : m_data(std::move(li)) {}

    constexpr basic_series(amt::SeriesView auto &&s, DType auto dtype)
        : m_data(s.size()), m_name(s.name()), m_dtype(get<value_type>(dtype)) {
        parallel_copy(s.begin(), s.end(), begin(),
                      [m_dtype = m_dtype](auto &&val) {
                          auto temp = val;
                          temp.dtype(m_dtype);
                          return temp;
                      });
    }

    constexpr basic_series(amt::SeriesView auto &&s)
        : m_data(s.size()), m_name(s.name()) {
        parallel_copy(
            s.begin(), s.end(), begin(),
            [m_dtype = m_dtype](auto &&val) { return value_type(val); });
    }

    constexpr basic_series(std::string_view name, base_type li)
        : m_data(std::move(li)), m_name(name) {}

    template <typename U>
    requires std::is_constructible_v<value_type, U> constexpr iterator
    insert(const_iterator pos, U u) {
        u.dtype(m_dtype);
        auto temp = value_type(std::move(u));
        return m_data.insert(pos, std::move(temp));
    }

    template <typename InputIt>
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
        return m_data.insert(pos, first, last);
    }

    constexpr iterator insert(const_iterator pos,
                              std::initializer_list<value_type> li) {

        for (auto &el : li) {
            el.dtype(m_dtype);
        }
        return m_data.insert(pos, std::move(li));
    }

    template <typename T>
    requires(!std::is_same_v<value_type, T>) constexpr iterator
        insert(const_iterator pos, std::initializer_list<T> li) {
        auto it = m_data.insert(pos, std::move(li));
        auto sz = std::distance(li.begin(), li.end());
        for (auto i = it; i != (it + sz); ++i) {
            i->dtype(m_dtype);
        }
        return it;
    }

    template <typename T, typename... Args>
    constexpr iterator emplace(const_iterator pos, Args &&... args) {
        value_type temp(T(std::forward<Args>(args)...));
        temp.dtype(m_dtype);
        return m_data.emplace(pos, std::move(temp));
    }

    constexpr iterator erase(const_iterator pos) { m_data.erase(pos); }

    constexpr iterator erase(const_iterator first, const_iterator last) {
        m_data.erase(first, last);
    }

    constexpr void pop_back() { m_data.pop_back(); }

    template <typename U>
    requires std::is_constructible_v<value_type, U> constexpr void
    push_back(U u) {
        if constexpr (is_box_v<value_type>) {
            u.dtype(m_dtype);
            m_data.push_back(std::move(u));
        } else {
            m_data.push_back(value_type(std::move(u), m_dtype));
        }
    }

    template <typename T, typename... Args>
    constexpr void emplace_back(Args &&... args) {
        value_type temp(T(std::forward<Args>(args)...));
        temp.dtype(m_dtype);
        m_data.emplace_back(std::move(temp));
    }

    constexpr iterator begin() noexcept { return m_data.begin(); }

    constexpr iterator end() noexcept { return m_data.end(); }

    constexpr const_iterator begin() const noexcept { return m_data.begin(); }

    constexpr const_iterator end() const noexcept { return m_data.end(); }

    constexpr reverse_iterator rbegin() noexcept { return m_data.rbegin(); }

    constexpr reverse_iterator rend() noexcept { return m_data.rend(); }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return m_data.rbegin();
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return m_data.rend();
    }

    template <typename U>
    requires std::is_constructible_v<value_type, U> constexpr void
    resize(size_type sz, U u) {
        if constexpr (is_box_v<U>) {
            u.dtype(m_dtype);
            m_data.resize(sz, value_type(std::move(u)));
        } else {
            m_data.resize(sz, value_type(std::move(u), m_dtype));
        }
    }

    constexpr void resize(size_type sz) { m_data.resize(sz, value_type(m_dtype)); }

    constexpr void reserve(size_type sz) { m_data.reserve(sz); }

    constexpr void clear() { m_data.clear(); }

    constexpr reference back() { return m_data.back(); }

    constexpr const_reference back() const { return m_data.back(); }

    constexpr pointer data() { return m_data.data(); }

    constexpr const_pointer data() const { return m_data.data(); }

    constexpr reference operator[](size_type k) { return m_data[k]; }

    constexpr const_reference operator[](size_type k) const {
        return m_data[k];
    }

    constexpr basic_series operator[](Series auto &&s) const {
        return filter(*this, s);
    }

    constexpr reference at(size_type k) { return m_data.at(k); }

    constexpr const_reference at(size_type k) const { return m_data.at(k); }

    constexpr size_type size() const noexcept { return m_data.size(); }

    constexpr bool empty() const noexcept { return m_data.empty(); }

    constexpr std::string_view name() const noexcept { return m_name; }
    
    constexpr std::string_view name() noexcept { return m_name; }

    constexpr void name(std::string_view name) { m_name = std::string(name); }

    constexpr view_type operator()(slice_type sl = {}) {
        return {m_data.data(), &m_name, m_data.size(), m_dtype, sl};
    }

    constexpr const_view_type operator()(slice_type sl = {}) const {
        return {m_data.data(), &m_name, m_data.size(), m_dtype, sl};
    }

    constexpr void reset_dtype() noexcept {
        m_dtype = ::amt::dtype<>();
        for (auto &el : *this) {
            el.reset_dtype();
        }
    }

    constexpr void dtype(DType auto d) {
        m_dtype = ::amt::dtype<>(get<value_type>(d));
        for (auto &el : *this) {
            el.dtype(m_dtype);
        }
    }

    constexpr size_type index() const noexcept { return m_dtype.index; }

    constexpr ::amt::dtype<> dtype() const noexcept { return m_dtype; }

  private:
   friend constexpr void detail::set_dtype(Series auto&, DType auto) noexcept;

  private:
    base_type m_data;
    name_type m_name{};
    ::amt::dtype<> m_dtype{};
};

using series = basic_series<box>;

} // namespace amt

std::ostream &operator<<(std::ostream &os, amt::Series auto const &s) {
    std::string_view temp = "( Name: ";
    using series_type = std::decay_t<decltype(s)>;
    if constexpr( amt::is_series_view_v<series_type> ){
        if constexpr( std::is_same_v< typename series_type::axis_tag, amt::tag::row_t > ){
            temp = "( Row Name: ";
        }else{
            temp = "( Column Name: ";
        }
    }
    if (s.empty()) {
        return os << temp << amt::beautify(s.name()) << ", Type: "
                  << "none )[ ]";
    } else {
        os << temp << amt::beautify(s.name())
           << ", Type: " << amt::type_to_string(s) << " )[ ";
        for (auto i = 0u; i < s.size() - 1; i++)
            os << s[i] << ", ";
        return os << s.back() << " ]";
    }
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr amt::
    basic_series<typename SeriesLHS::value_type>
    operator<(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(
            ERR_CSTR("operator<(SeriesLHS const&, SeriesRHS const&) : "
                     "size mismatch"));
    }
    auto sz = lhs.size();
    using value_type = typename SeriesLHS::value_type;
    amt::basic_series<value_type> temp(lhs.name(), sz, false,
                                       amt::dtype<bool>{});

    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto const &l, auto const &r) { return l < r; });

    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr amt::
    basic_series<typename SeriesLHS::value_type>
    operator>(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    return rhs < lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr amt::
    basic_series<typename SeriesLHS::value_type>
    operator==(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(
            ERR_CSTR("operator==(SeriesLHS const&, SeriesRHS const&) : "
                     "size mismatch"));
    }
    auto sz = lhs.size();
    using value_type = typename SeriesLHS::value_type;
    amt::basic_series<value_type> temp(lhs.name(), sz, false,
                                       amt::dtype<bool>{});

    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto const &l, auto const &r) { return l == r; });

    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr amt::
    basic_series<typename SeriesLHS::value_type>
    operator!=(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(
            ERR_CSTR("operator!=(SeriesLHS const&, SeriesRHS const&) : "
                     "size mismatch"));
    }
    auto sz = lhs.size();
    using value_type = typename SeriesLHS::value_type;
    amt::basic_series<value_type> temp(lhs.name(), sz, false,
                                       amt::dtype<bool>{});

    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto const &l, auto const &r) { return l != r; });

    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr amt::
    basic_series<typename SeriesLHS::value_type>
    operator>=(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(
            ERR_CSTR("operator>=(SeriesLHS const&, SeriesRHS const&) : "
                     "size mismatch"));
    }
    auto sz = lhs.size();
    using value_type = typename SeriesLHS::value_type;
    amt::basic_series<value_type> temp(lhs.name(), sz, false,
                                       amt::dtype<bool>{});

    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto const &l, auto const &r) { return l >= r; });

    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr amt::
    basic_series<typename SeriesLHS::value_type>
    operator<=(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    return rhs >= lhs;
}
template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator+(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(ERR_CSTR(
            "operator+(SeriesLHS const&, SeriesRHS const&) : size mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l + r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator-(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(ERR_CSTR(
            "operator-(SeriesLHS const&, SeriesRHS const&) : size mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l - r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator*(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(ERR_CSTR(
            "operator*(SeriesLHS const&, SeriesRHS const&) : size mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l * r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator/(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(ERR_CSTR(
            "operator/(SeriesLHS const&, SeriesRHS const&) : size mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l / r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator%(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(ERR_CSTR(
            "operator%(SeriesLHS const&, SeriesRHS const&) : size mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l % r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator&(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(ERR_CSTR(
            "operator&(SeriesLHS const&, SeriesRHS const&) : size mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l & r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator|(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(ERR_CSTR(
            "operator|(SeriesLHS const&, SeriesRHS const&) : size mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l | r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator^(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(ERR_CSTR(
            "operator^(SeriesLHS const&, SeriesRHS const&) : size mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l ^ r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator<<(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(
            ERR_CSTR("operator<<(SeriesLHS const&, SeriesRHS const&) : size "
                     "mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l << r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator>>(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(
            ERR_CSTR("operator>>(SeriesLHS const&, SeriesRHS const&) : size "
                     "mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l >> r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator&&(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(
            ERR_CSTR("operator&&(SeriesLHS const&, SeriesRHS const&) : size "
                     "mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l && r; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto
operator||(SeriesLHS const &lhs, SeriesRHS const &rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::runtime_error(
            ERR_CSTR("operator||(SeriesLHS const&, SeriesRHS const&) : size "
                     "mismatch"));
    }
    using return_type =
        amt::basic_series<std::decay_t<typename SeriesLHS::value_type>>;
    return_type temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), rhs.begin(), temp.begin(),
                       [](auto &&l, auto &&r) { return l || r; });
    return temp;
}

template <amt::Series SeriesType>
constexpr auto operator!(SeriesType const &lhs) {
    SeriesType temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), temp.begin(),
                       [](auto &&l) { return !l; });
    return temp;
}

template <amt::Series SeriesType>
constexpr auto operator+(SeriesType const &lhs) {
    SeriesType temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), temp.begin(),
                       [](auto &&l) { return +l; });
    return temp;
}

template <amt::Series SeriesType>
constexpr auto operator-(SeriesType const &lhs) {
    SeriesType temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), temp.begin(),
                       [](auto &&l) { return -l; });
    return temp;
}

template <amt::Series SeriesType>
constexpr auto operator~(SeriesType const &lhs) {
    SeriesType temp(lhs.name(), lhs.size(), lhs.dtype());
    parallel_transform(lhs.begin(), lhs.end(), temp.begin(),
                       [](auto &&l) { return ~l; });
    return temp;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator+=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs + rhs;
    lhs = std::move(temp);
    return lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator-=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs - rhs;
    lhs = std::move(temp);
    return lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator*=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs * rhs;
    lhs = std::move(temp);
    return lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator/=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs / rhs;
    lhs = std::move(temp);
    return lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator%=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs % rhs;
    lhs = std::move(temp);
    return lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator|=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs | rhs;
    lhs = std::move(temp);
    return lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator&=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs & rhs;
    lhs = std::move(temp);
    return lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator^=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs ^ rhs;
    lhs = std::move(temp);
    return lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator<<=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs << rhs;
    lhs = std::move(temp);
    return lhs;
}

template <amt::Series SeriesLHS, amt::Series SeriesRHS>
requires(amt::is_series_same_v<SeriesLHS, SeriesRHS>) constexpr auto &
operator>>=(SeriesLHS &lhs, SeriesRHS const &rhs) {
    auto temp = lhs >> rhs;
    lhs = std::move(temp);
    return lhs;
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator==(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator==(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator==(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator!=(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator!=(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator!=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator<(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator<(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator<(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator>(LHS const &lhs, RHS const &rhs) {
    return rhs < lhs;
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator<=(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator<=(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator<=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator>=(LHS const &lhs, RHS const &rhs) {
    return rhs <= lhs;
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator+(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator+(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator+(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator-(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator-(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator-(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator*(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator*(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator*(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator/(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator/(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator/(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator%(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator%(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator%(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(!amt::SeriesView<LHS> &&
         static_cast<bool>(amt::Series<RHS> ^
                           amt::Series<LHS>)) constexpr decltype(auto)
operator+=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator+=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator+=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(!amt::SeriesView<LHS> &&
         static_cast<bool>(amt::Series<RHS> ^
                           amt::Series<LHS>)) constexpr decltype(auto)
operator-=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator-=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator-=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(!amt::SeriesView<LHS> &&
         static_cast<bool>(amt::Series<RHS> ^
                           amt::Series<LHS>)) constexpr decltype(auto)
operator*=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator*=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator*=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(!amt::SeriesView<LHS> &&
         static_cast<bool>(amt::Series<RHS> ^
                           amt::Series<LHS>)) constexpr decltype(auto)
operator/=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator/=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator/=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(!amt::SeriesView<LHS> &&
         static_cast<bool>(amt::Series<RHS> ^
                           amt::Series<LHS>)) constexpr decltype(auto)
operator%=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator%=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator%=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator&(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator&(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator&(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator|(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator|(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator|(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator^(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator^(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator^(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator&=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator&=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator&=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator|=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator|=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator|=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator^=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator^=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator^=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator&&(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator&&(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator&&(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator||(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator||(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator||(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires( !amt::is_std_basic_ostream_v<LHS> && static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator<<(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator<<(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator<<(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator>>(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        return ::operator>>(lhs, temp);
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator>>(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator<<=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator<<=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator<<=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Series<RHS> ^ amt::Series<LHS>)) constexpr auto
operator>>=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Series<LHS>) {
        LHS temp(lhs.name(), lhs.size(), rhs, lhs.dtype());
        ::operator>>=(lhs, temp);
        return lhs;
    } else {
        RHS temp(rhs.name(), rhs.size(), lhs, rhs.dtype());
        return ::operator>>=(temp, rhs);
    }
}

#endif // AMT_DATAFRAME_SERIES_HPP
