#if !defined(AMT_DATAFRAME_FRAME_HPP)
#define AMT_DATAFRAME_FRAME_HPP

#include <dataframe/frame_utils.hpp>
#include <dataframe/frame_view.hpp>
#include <dataframe/series.hpp>

namespace amt {

// Stores data in column major
template <Box BoxType> struct basic_frame<BoxType> {

    using box_type = BoxType;
    using value_type = basic_series<box_type>;
    using name_type = typename value_type::name_type;
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
    using slice_type = basic_slice<size_type>;
    using shape_type = std::pair<size_type, size_type>;
    using row_view_type = basic_row_view<value_type>;
    using const_row_view_type = basic_row_view<value_type const>;
    using col_view_type = basic_column_view<box_type>;
    using const_col_view_type = basic_column_view<box_type const>;
    using frame_view_type = basic_frame_view<value_type>;
    using const_frame_view_type = basic_frame_view<value_type const>;

    constexpr basic_frame() = default;
    constexpr basic_frame(basic_frame const &other) = default;
    constexpr basic_frame(basic_frame &&other) = default;
    constexpr basic_frame &operator=(basic_frame const &other) = default;
    constexpr basic_frame &operator=(basic_frame &&other) = default;
    constexpr ~basic_frame() = default;

    constexpr basic_frame(size_type cols) : m_data(cols) {}

    template <FrameView FrameType>
    requires(is_frame_same_v<FrameType, basic_frame>) constexpr basic_frame(
        FrameType &&fv)
        : basic_frame(fv.cols()) {
        std::copy(fv.begin(), fv.end(), begin());
    }

    constexpr basic_frame(size_type cols, size_type rows)
        : m_data(cols, value_type(rows)) {
        generate_name();
    }

    constexpr basic_frame(shape_type s)
        : basic_frame(std::get<0>(s), std::get<1>(s)) {}

    constexpr basic_frame(size_type cols, size_type rows, DType auto dtype)
        : m_data(cols, value_type(rows, dtype)) {
        generate_name();
    }

    template <typename U>
    requires(std::is_constructible_v<box_type, U> &&
             !DType<U>) constexpr basic_frame(size_type cols, size_type rows,
                                              U u, DType auto dtype)
        : m_data(cols, value_type(rows, std::move(u), dtype)) {
        generate_name();
    }

    template <typename U>
    requires(std::is_constructible_v<box_type, U> &&
             !DType<U>) constexpr basic_frame(shape_type s, U u,
                                              DType auto dtype)
        : basic_frame(std::get<0>(s), std::get<1>(s), std::move(u), dtype) {}

    template <typename U>
    requires(std::is_constructible_v<box_type, U> &&
             !DType<U>) constexpr basic_frame(shape_type s, U u)
        : basic_frame(std::get<0>(s), std::get<1>(s), std::move(u)) {}

    template <typename U>
    requires(std::is_constructible_v<box_type, U> &&
             !DType<U>) constexpr basic_frame(size_type cols, size_type rows,
                                              U u)
        : m_data(cols, value_type(rows, std::move(u))) {
        generate_name();
    }

    constexpr basic_frame(shape_type s, DType auto dtype)
        : basic_frame(std::get<0>(s), std::get<1>(s), dtype) {}

    constexpr basic_frame(size_type cols, value_type val)
        : m_data(cols, std::move(val)) {}

    constexpr basic_frame(std::initializer_list<value_type> li)
        : m_data(std::move(li)) {
        if (!all_cols_have_same_rows()) {
            throw std::length_error(
                ERR_CSTR("amt::basic_frame(std::initializer_list<value_type>) "
                         ": all cols should have same number of rows"));
        }
    }

    constexpr basic_frame(base_type li) : m_data(std::move(li)) {}

    constexpr iterator col_insert(const_iterator pos, value_type u) {
        if (!empty() && u.size() != rows()) {
            throw std::length_error(
                ERR_CSTR("amt::basic_frame::insert(const_iterator, value_type) "
                         ": rows mismatch"));
        }
        return m_data.insert(pos, std::move(u));
    }

    constexpr iterator insert(const_iterator pos, value_type u, tag::col_t) {
        return col_insert(pos, std::move(u));
    }

    template <typename InputIt>
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last,
                              tag::col_t) {
        return col_insert(pos, first, last);
    }

    template <typename InputIt>
    constexpr iterator insert(size_type pos, InputIt first, InputIt last,
                              tag::col_t) {
        return col_insert(pos, first, last);
    }

    template <typename InputIt>
    constexpr void insert(size_type pos, InputIt first, InputIt last,
                          tag::row_t) {
        return row_insert(pos, first, last);
    }

    template <typename InputIt>
    constexpr iterator col_insert(size_type pos, InputIt first, InputIt last) {
        return col_insert(begin() + pos, first, last);
    }

    template <typename InputIt>
    constexpr iterator col_insert(const_iterator pos, InputIt first,
                                  InputIt last) {
        if (!empty() &&
            static_cast<size_type>(std::distance(first, last)) != rows()) {
            throw std::length_error(
                ERR_CSTR("amt::basic_frame::col_insert(const_iterator, "
                         "InputIt, InputIt) : rows mismatch"));
        }
        return m_data.insert(pos, first, last);
    }

    template <typename InputIt>
    constexpr void row_insert(size_type pos, InputIt first, InputIt last) {
        if (static_cast<size_type>(std::distance(first, last)) != cols()) {
            throw std::length_error(
                ERR_CSTR("amt::basic_frame::row_insert(size_type, InputIt, "
                         "InputIt) : column size mismatch"));
        }
        for (auto &el : m_data) {
            el.insert(el.begin() + pos, first, first + 1);
            ++first;
        }
    }

    template <typename T>
    constexpr iterator insert(size_type pos, std::initializer_list<T> li,
                              tag::col_t) {
        return col_insert(pos, std::move(li));
    }

    template <typename T>
    constexpr iterator insert(size_type pos, std::initializer_list<T> li,
                              tag::row_t) {
        return row_insert(pos, std::move(li));
    }

    template <typename T>
    constexpr iterator insert(const_iterator pos, std::initializer_list<T> li,
                              tag::col_t) {
        return col_insert(pos, std::move(li));
    }

    template <typename T>
    constexpr void row_insert(size_type pos, std::initializer_list<T> li) {
        if (li.size() != cols()) {
            throw std::length_error(
                ERR_CSTR("amt::basic_frame::row_insert(size_type, "
                         "std::initializer_list<T> "
                         ") : column size mismatch"));
        }
        auto first = li.begin();
        for (auto &el : m_data) {
            el.insert(el.begin() + pos, std::move(*first));
            ++first;
        }
    }

    template <typename T>
    constexpr iterator col_insert(size_type pos, std::initializer_list<T> li) {
        return col_insert(begin() + pos, std::move(li));
    }

    template <typename T>
    constexpr iterator col_insert(const_iterator pos,
                                  std::initializer_list<T> li) {
        if (!empty() && li.size() != rows()) {
            throw std::length_error(
                ERR_CSTR("amt::basic_frame::col_insert(const_iterator, "
                         "std::initializer_list<T>) : rows mismatch"));
        }
        return m_data.insert(pos, std::move(li));
    }

    constexpr iterator erase(const_iterator pos, tag::col_t) { col_erase(pos); }

    constexpr iterator erase(size_type pos, tag::col_t) { col_erase(pos); }

    constexpr void erase(size_type pos, tag::row_t) { row_erase(pos); }

    constexpr iterator col_erase(const_iterator pos) { m_data.erase(pos); }

    constexpr iterator col_erase(size_type pos) { m_data.erase(begin() + pos); }

    constexpr void row_erase(size_type pos) {
        for (auto &el : m_data) {
            el.erase(el.begin() + pos);
        }
    }

    constexpr iterator erase(const_iterator first, const_iterator last,
                             tag::col_t) {
        col_erase(first, last);
    }

    constexpr iterator erase(size_type first, size_type last, tag::col_t) {
        col_erase(first, last);
    }

    constexpr iterator erase(const_iterator first, const_iterator last,
                             tag::row_t) {
        row_erase(first, last);
    }

    constexpr iterator col_erase(size_type first, size_type last) {
        col_erase(begin() + first, begin() + last);
    }

    constexpr iterator col_erase(const_iterator first, const_iterator last) {
        m_data.erase(first, last);
    }

    constexpr void row_erase(size_type first, size_type last) {
        for (auto &el : m_data) {
            el.erase(el.begin() + first, el.begin() + last);
        }
    }

    constexpr void pop_back(tag::col_t) { m_data.col_pop_back(); }
    constexpr void pop_back(tag::row_t) { m_data.row_pop_back(); }

    constexpr void col_pop_back() { m_data.pop_back(); }

    constexpr void row_pop_back() {
        if (empty())
            return;
        for (auto &el : m_data) {
            el.pop_back();
        }
    }

    constexpr void push_back(Series auto u, tag::col_t) {
        col_push_back(std::move(u));
    }
    constexpr void push_back(Series auto u, tag::row_t) {
        row_push_back(std::move(u));
    }

    constexpr void col_push_back(Series auto u) {
        if (!empty() && u.size() != rows()) {
            throw std::length_error(ERR_CSTR("amt::basic_frame::col_push_back("
                                             "Series auto) : rows mismatch"));
        }
        m_data.push_back(std::move(u));
    }

    constexpr void row_push_back(Series auto u) {
        if (empty()) {
            resize(u.size());
        }
        if (cols() != u.size()) {
            throw std::length_error(ERR_CSTR(
                "amt::basic_frame::row_push_back(Series auto) : column "
                "size mismatch"));
        }
        for (auto i = 0u; i < cols(); ++i) {
            m_data[i].push_back(std::move(u[i]));
        }
    }

    constexpr void resize(size_type sz, value_type u) {
        m_data.resize(sz, std::move(u));
    }

    constexpr void resize(size_type cols, size_type rows, box_type b) {
        m_data.resize(cols, value_type(rows, b));
        for (auto i = 0u; i < cols; ++i)
            m_data[i].resize(rows, std::move(b));
    }

    constexpr void resize(size_type cols, size_type rows) {
        m_data.resize(cols, value_type(rows));
        for (auto i = 0u; i < cols; ++i)
            m_data[i].resize(rows);
    }

    constexpr void resize(shape_type s, box_type b) {
        resize(std::get<0>(s), std::get<1>(s), std::move(b));
    }

    constexpr void resize(shape_type s) {
        resize(std::get<0>(s), std::get<1>(s));
    }

    constexpr void reserve(size_type cols, size_type rows) {
        m_data.reserve(cols);
        for (auto i = 0u; i < cols; ++i)
            m_data[i].reserve(rows);
    }

    constexpr void reserve(shape_type s) {
        reserve(std::get<0>(s), std::get<1>(s));
    }

    constexpr void resize(size_type sz) { m_data.resize(sz); }

    constexpr void reserve(size_type sz) { m_data.reserve(sz); }

    constexpr void shrink_to_fit() {
        m_data.shrink_to_fit();
        for (auto &el : m_data)
            el.shrink_to_fit();
    }

    constexpr void clear() { m_data.clear(); }

    constexpr reference back() { return m_data.back(); }

    constexpr const_reference back() const { return m_data.back(); }

    constexpr shape_type shape() const noexcept { return {cols(), rows()}; }

    constexpr bool empty() const noexcept { return m_data.empty(); }

    constexpr size_type size() const noexcept { return m_data.size(); }

    constexpr size_type cols() const noexcept { return m_data.size(); }

    constexpr pointer data() noexcept { return m_data.data(); }

    constexpr const_pointer data() const noexcept { return m_data.data(); }

    constexpr size_type rows() const noexcept {
        return empty() ? 0u : m_data[0].size();
    }

    constexpr void dtype(size_type k, DType auto d) { m_data[k].dtype(d); }

    constexpr void dtype(DType auto d) {
        for (auto i = 0u; i < cols(); ++i) {
            dtype(i, d);
        }
    }

    constexpr ::amt::dtype<> dtype(size_type k) const {
        return m_data[k].dtype();
    }

    constexpr reference operator[](size_type k) { return m_data[k]; }

    constexpr const_reference operator[](size_type k) const {
        return m_data[k];
    }

    constexpr reference operator[](std::string_view k) {
        auto it = std::find_if(begin(), end(),
                               [&](auto const &s) { return s.name() == k; });
        if (it == end()) {
            throw std::runtime_error(
                ERR_CSTR("amt::basic_frame::operator[](std::string_view) : "
                         "column name not found"));
        }
        return *it;
    }

    constexpr const_reference operator[](std::string_view k) const {
        auto it = std::find_if(begin(), end(),
                               [&](auto const &s) { return s.name() == k; });
        if (it == end()) {
            throw std::runtime_error(
                ERR_CSTR("amt::basic_frame::operator[](std::string_view) : "
                         "column name not found"));
        }
        return *it;
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

    constexpr col_view_type get_col(size_type k) { return m_data[k](); }

    constexpr const_col_view_type get_col(size_type k) const {
        return m_data[k]();
    }

    constexpr row_view_type get_row(size_type k) {
        return {m_data.data(), k, cols()};
    }

    constexpr const_row_view_type get_row(size_type k) const {
        return {m_data.data(), k, cols()};
    }

    constexpr box_type &operator()(size_type c, size_type r) {
        return m_data[c][r];
    }

    constexpr box_type const &operator()(size_type c, size_type r) const {
        return m_data[c][r];
    }

    constexpr reference at(size_type k) { return m_data.at(k); }

    constexpr const_reference at(size_type k) const { return m_data.at(k); }

    constexpr reference at(std::string_view k) { return m_data[k]; }

    constexpr const_reference at(std::string_view k) const { return m_data[k]; }

    constexpr box_type &at(size_type c, size_type r) {
        return m_data.at(c).at(r);
    }

    constexpr box_type const &at(size_type c, size_type r) const {
        return m_data.at(c).at(r);
    }

    std::vector<::amt::dtype<>> vdtype() const {
        std::vector<::amt::dtype<>> temp(cols());
        for (auto i = 0u; i < temp.size(); ++i)
            temp[i] = dtype(i);
        return temp;
    }

    constexpr void set_dtypes(std::vector<::amt::dtype<>> const &v) const {
        for (auto i = 0u; i < v.size(); ++i)
            dtype(i, v[i]);
    }

    constexpr void set_dtypes(Frame auto &&f) {
        for (auto i = 0u; i < f.cols(); ++i)
            dtype(i, f.dtype(i));
    }

    constexpr std::string_view name(size_type k) const {
        return m_data[k].name();
    }

    constexpr void name(size_type k, std::string_view name) const {
        return m_data[k].name(name);
    }

    constexpr iterator begin() { return m_data.begin(); }

    constexpr iterator end() { return m_data.end(); }

    constexpr const_iterator begin() const { return m_data.begin(); }

    constexpr const_iterator end() const { return m_data.end(); }

    constexpr reverse_iterator rbegin() { return m_data.rbegin(); }

    constexpr reverse_iterator rend() { return m_data.rend(); }

    constexpr const_reverse_iterator rbegin() const { return m_data.rbegin(); }

    constexpr const_reverse_iterator rend() const { return m_data.rend(); }

    constexpr frame_view_type operator()(slice_type col_slice = {},
                                         slice_type row_slice = {}) {
        return {data(), cols(), rows(), std::move(col_slice),
                std::move(row_slice)};
    }

    constexpr const_frame_view_type
    operator()(slice_type col_slice = {}, slice_type row_slice = {}) const {
        return {data(), cols(), rows(), std::move(col_slice),
                std::move(row_slice)};
    }

    name_list vnames() const {
        name_list temp;
        temp.reserve(cols());
        for (auto const &el : m_data) {
            temp.push_back(el.name());
        }
        return temp;
    }

    void set_names(std::vector<std::string> vec) {
        auto sz = std::min(vec.size(), cols());
        for (auto i = 0u; i < sz; ++i) {
            m_data[i].name(std::move(vec[i]));
        }
    }

    void set_names(value_type vec) {
        auto sz = std::min(vec.size(), cols());
        for (auto i = 0u; i < sz; ++i) {
            m_data[i].name(std::move(get<std::string>(vec[i])));
        }
    }

    void set_names(name_list vec) {
        auto sz = std::min(vec.size(), cols());
        for (auto i = 0u; i < sz; ++i) {
            m_data[i].name(std::move(vec[i]));
        }
    }

    void set_names(Frame auto &&f) {
        auto sz = std::min(f.cols(), cols());
        for (auto i = 0u; i < sz; ++i) {
            m_data[i].name(f.name(i));
        }
    }

  private:
    void generate_name() {
        for (auto i = 0u; i < cols(); ++i) {
            m_data[i].name(std::to_string(i));
        }
    }

    constexpr bool all_cols_have_same_rows() const noexcept {
        auto sz = rows();
        for (auto i = 1u; i < cols(); ++i) {
            if (sz != m_data[i].size())
                return false;
        }
        return true;
    }

  private:
    base_type m_data;
};

using frame = basic_frame<box>;

} // namespace amt

std::ostream &operator<<(std::ostream &os, amt::Frame auto &&f) {
    if (f.empty()) {
        return os << "{ }";
    } else {
        os << "{\n";
        for (auto const &el : f) {
            os << "    " << el << '\n';
        }
        os << "}";
    }
    return os;
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator==(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator==(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }

    using return_type = amt::frame_result_t<FrameLHS, FrameRHS>;
    return_type ret(lhs.shape(), amt::dtype<bool>());
    ret.set_names(lhs);
    for (auto i = 0u; i < lhs.cols(); ++i) {
        ret[i] = std::move(lhs[i] == rhs[i]);
    }
    return ret;
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator!=(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator!=(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }

    using return_type = amt::frame_result_t<FrameLHS, FrameRHS>;
    return_type ret(lhs.shape(), amt::dtype<bool>());
    for (auto i = 0u; i < lhs.cols(); ++i) {
        ret[i] = std::move(lhs[i] != rhs[i]);
    }
    return ret;
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator<(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator<(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }

    using return_type = amt::frame_result_t<FrameLHS, FrameRHS>;
    return_type ret(lhs.shape(), amt::dtype<bool>());
    for (auto i = 0u; i < lhs.cols(); ++i) {
        ret[i] = std::move(lhs[i] < rhs[i]);
    }
    return ret;
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator>(FrameLHS const &lhs, FrameRHS const &rhs) {
    return rhs < lhs;
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator<=(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator<=(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }

    using return_type = amt::frame_result_t<FrameLHS, FrameRHS>;
    return_type ret(lhs.shape(), amt::dtype<bool>());
    for (auto i = 0u; i < lhs.cols(); ++i) {
        ret[i] = std::move(lhs[i] <= rhs[i]);
    }
    return ret;
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator>=(FrameLHS const &lhs, FrameRHS const &rhs) {
    return rhs <= lhs;
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator+(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator+(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        res[i] = rhs[i] + lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator-(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator-(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        res[i] = rhs[i] - lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator*(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator*(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        res[i] = rhs[i] * lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator/(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator/(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        res[i] = rhs[i] / lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator+=(FrameLHS &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator+=(FrameLHS &, FrameRHS const&) : size mismatch"));
    }

    for (auto i = 0u; i < rhs.cols(); ++i) {
        rhs[i] += lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator-=(FrameLHS &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator-=(FrameLHS &, FrameRHS const&) : size mismatch"));
    }

    for (auto i = 0u; i < rhs.cols(); ++i) {
        rhs[i] -= lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator*=(FrameLHS &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator*=(FrameLHS &, FrameRHS const&) : size mismatch"));
    }

    for (auto i = 0u; i < rhs.cols(); ++i) {
        rhs[i] *= lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator/=(FrameLHS &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator/=(FrameLHS &, FrameRHS const&) : size mismatch"));
    }

    for (auto i = 0u; i < rhs.cols(); ++i) {
        rhs[i] /= lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator&&(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator&&(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        res[i] = (rhs[i] && lhs[i]);
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator||(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator||(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        res[i] = (rhs[i] || lhs[i]);
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator&(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator&(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        res[i] = (rhs[i] & lhs[i]);
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator^(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator^(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        res[i] = (rhs[i] ^ lhs[i]);
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator|(FrameLHS const &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator|(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        res[i] = (rhs[i] | lhs[i]);
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator&=(FrameLHS &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator&=(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        rhs[i] &= lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator^=(FrameLHS &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator^=(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        rhs[i] ^= lhs[i];
    }
}

template <amt::Frame FrameLHS, amt::Frame FrameRHS>
constexpr amt::frame_result_t<FrameLHS, FrameRHS>
operator|=(FrameLHS &lhs, FrameRHS const &rhs) {
    if (rhs.shape() != lhs.shape()) {
        throw std::runtime_error(ERR_CSTR(
            "operator|=(FrameLHS const&, FrameRHS const&) : size mismatch"));
    }
    amt::frame_result_t<FrameLHS, FrameRHS> res(rhs.cols());
    for (auto i = 0u; i < rhs.cols(); ++i) {
        rhs[i] |= lhs[i];
    }
}

#endif // AMT_DATAFRAME_FRAME_HPP
