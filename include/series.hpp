#if !defined(AMT_SERIES_HPP)
#define AMT_SERIES_HPP

#include <box.hpp>
#include <initializer_list>
#include <series_view.hpp>
#include <vector>

namespace amt {

template <typename... Ts> struct series {
    using value_type = box<Ts...>;
    using name_type = std::string;
    using base_type = std::vector<value_type>;
    using size_type = typename base_type::size_type;
    using reference = typename base_type::reference;
    using const_reference = typename base_type::const_reference;
    using pointer = typename base_type::pointer;
    using const_pointer = typename base_type::const_pointer;
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using reverse_iterator = typename base_type::reverse_iterator;
    using const_reverse_iterator = typename base_type::const_reverse_iterator;

    // static constexpr auto const is_packed = value_type::is_packed;

    constexpr series() = default;
    constexpr series(series const &other)
        : m_data(other.m_data), m_name(other.m_name) {}
    constexpr series(series &&other) noexcept
        : m_data(std::move(other.m_data)), m_name(std::move(other.m_name)) {}

    constexpr series &operator=(series const &other) = default;
    constexpr series &operator=(series &&other) noexcept = default;

    ~series() = default;

    template <typename T>
    constexpr series(size_type sz, T &&val, std::string_view name = "")
        : m_data(sz, std::move(val)), m_name(name) {}

    constexpr series(size_type sz, std::string_view name = "")
        : m_data(sz), m_name(name) {}

    template <View V>
    requires Series<typename V::view_of> constexpr series(V v)
        : m_data(v.size()), m_name(v.name()) {
#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < v.size(); ++i) {
            m_data[i] = v[i];
        }
    }

    template <typename T>
    constexpr series(std::initializer_list<T> li)
        : series(std::move(std::vector<T>(std::move(li)))) {}

    template <typename T>
    constexpr series(std::string_view name, std::initializer_list<T> li)
        : series(name, std::move(std::vector<T>(std::move(li)))) {}

    template <typename T>
    constexpr series(std::vector<T> v) : series("", std::move(v)) {}

    template <typename T>
    constexpr series(std::string_view name, std::vector<T> v)
        : m_data(v.size()), m_name(name) {

        bool flag = false;

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < v.size(); ++i) {
            m_data[i] = std::move(v[i]);
            if constexpr (is_box_v<T>) {
                flag |= (m_data[0].index() != m_data[i].index());
                if (flag)
                    break;
            }
        }

        if constexpr (is_box_v<T>) {
            if (flag) {
                throw std::runtime_error(
                    "amt::series(std::vector<T>) : "
                    "all the elements in the series should have same type");
            }
        }
    }

    template <typename T, typename... Args> void emplace_back(Args &&... args) {
        push_back(T(std::forward<Args>(args)...));
    }

    template <typename T> void push_back(T &&val) {
        m_data.push_back(std::move(val));

        if (m_data[0].index() != back().index()) {
            throw std::runtime_error(
                "amt::series::push_back(T&&) : "
                "all the elements in the series should have same type");
        }
    }

    [[nodiscard]] constexpr size_type size() const noexcept {
        return m_data.size();
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return m_data.empty();
    }

    [[nodiscard]] constexpr std::string const &name() const noexcept {
        return m_name;
    }

    [[nodiscard]] constexpr std::string &name() noexcept { return m_name; }

    [[nodiscard]] constexpr reference operator[](size_type k) {
        return m_data[k];
    }

    [[nodiscard]] constexpr const_reference operator[](size_type k) const {
        return m_data[k];
    }

    [[nodiscard]] constexpr reference at(size_type k) { return m_data.at(k); }

    [[nodiscard]] constexpr const_reference at(size_type k) const {
        return m_data.at(k);
    }

    [[nodiscard]] constexpr reference back() { return m_data.back(); }

    [[nodiscard]] constexpr const_reference back() const {
        return m_data.back();
    }

    [[nodiscard]] constexpr pointer data() noexcept { return m_data.data(); }

    [[nodiscard]] constexpr const_pointer data() const noexcept {
        return m_data.data();
    }

    [[nodiscard]] constexpr iterator begin() noexcept { return m_data.begin(); }

    [[nodiscard]] constexpr iterator end() noexcept { return m_data.end(); }

    [[nodiscard]] constexpr const_iterator begin() const noexcept {
        return m_data.begin();
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept {
        return m_data.end();
    }

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
        return m_data.rbegin();
    }

    [[nodiscard]] constexpr reverse_iterator rend() noexcept {
        return m_data.rend();
    }

    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
        return m_data.rbegin();
    }

    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
        return m_data.rend();
    }

    [[nodiscard]] iterator erase(iterator pos) { return m_data.erase(pos); }

    void erase(typename iterator::difference_type pos) {
        m_data.erase(m_data.begin() + pos);
    }

    [[nodiscard]] iterator erase(const_iterator pos) {
        return m_data.erase(pos);
    }

    [[nodiscard]] iterator erase(iterator first, iterator last) {
        return m_data.erase(first, last);
    }

    [[nodiscard]] iterator erase(const_iterator first, const_iterator last) {
        return m_data.erase(first, last);
    }

    void clear() { m_data.clear(); }

    void resize(size_type sz) { m_data.resize(sz); }

    constexpr void reserve(size_type cap) { m_data.reserve(cap); }

    template <typename T> void resize(size_type sz, T &&val) {
        if (!empty() && !back().empty()) {
            bool flag = false;

            if constexpr (is_box_v<T>) {
                flag = (back().index() != val.index());
            } else {
                flag = (back().index() != type_index<T>());
            }

            if (flag) {
                throw std::runtime_error(
                    "amt::series::resize(size_type,T&&) : "
                    "all the elements in the series should have same type");
            }
        }
        m_data.resize(sz, std::forward<T>(val));
    }

    [[nodiscard]] constexpr bool is_char() const noexcept {
        return !empty() && m_data.back().is_char();
    }

#define AMT_DEFINE_DEFAULT_TYPES(TYPE, MESS, NAME)                             \
    [[nodiscard]] constexpr bool is_##NAME() const noexcept {                  \
        return !empty() && m_data.back().is_##NAME();                          \
    }

#include <core/default_types.def>

    [[nodiscard]] constexpr bool is_integer() const noexcept {
        return is_int8() || is_int16() || is_int32() || is_int64() ||
               is_uint8() || is_uint16() || is_uint32() || is_uint64() ||
               is_size_t() || is_ptrdiff_t();
    }

    [[nodiscard]] constexpr bool is_floating_point() const noexcept {
        return is_float() || is_double();
    }

    template <typename T>[[nodiscard]] constexpr bool is_type() const noexcept {
        return !empty() && back().template is_type<T>();
    }

    template <typename T>
    [[nodiscard]] constexpr auto type_index() const noexcept {
        return value_type::template type_index<T>();
    }

    [[nodiscard]] constexpr auto check_types() const noexcept {
        for (auto const &el : *this) {
            if ((m_data[0].index() != el.index()) || el.empty()) {
                return false;
            }
        }
        return true;
    }

    template <typename T>
    [[nodiscard]] constexpr view<series, true>
    operator()(basic_slice<T> s) const noexcept {
        return {*this, std::move(s)};
    }

    template <typename T>
    [[nodiscard]] constexpr view<series, false>
    operator()(basic_slice<T> s) noexcept {
        return {*this, std::move(s)};
    }

    template <typename T, bool isConst>
    [[nodiscard]] constexpr view<series, isConst> operator()() noexcept {
        return {*this};
    }

  private:
    base_type m_data;
    name_type m_name{};
};

template <typename... Ts> std::string type_to_string(series<Ts...> const &s) {
    return s.empty() ? "None" : type_to_string(s[0]);
}

template <typename T>
[[nodiscard]] constexpr bool holds_alternative(Series auto &&s) noexcept {
    using type = std::decay_t<decltype(s)>;
    using storage_type = typename type::value_type;
    return storage_type::template type_index<T>() < storage_type::size;
}

} // namespace amt

#endif // AMT_SERIES_HPP
