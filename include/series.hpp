#if !defined(AMT_SERIES_HPP)
#define AMT_SERIES_HPP

#include <core/storage.hpp>
#include <initializer_list>
#include <series_view.hpp>
#include <vector>

namespace amt {

template <typename... Ts> struct series {
    using value_type = storage<Ts...>;
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

    constexpr series() = default;
    constexpr series(series const &other) : m_data(other.m_data) {}
    constexpr series(series &&other) noexcept
        : m_data(std::move(other.m_data)) {}

    constexpr series &operator=(series const &other) = default;
    constexpr series &operator=(series &&other) noexcept = default;

    ~series() = default;

    template <typename T>
    constexpr series(size_type sz, T &&val) : m_data(sz, std::move(val)) {}

    constexpr series(size_type sz) : m_data(sz) {}

    template <View V>
    requires Series<typename V::view_of> constexpr series(V v)
        : m_data(v.size()) {
        for (auto i = 0ul; i < v.size(); ++i) {
            m_data[i] = v[i];
        }
    }

    template <typename T>
    constexpr series(std::initializer_list<T> li)
        : series(std::move(std::vector<T>(std::move(li)))) {}

    template <typename T>
    constexpr series(std::vector<T> v) : m_data(v.size()) {
        size_type i{};
        for (auto &el : v) {
            m_data[i] = std::move(el);

            if (m_data[0].index() != m_data[i].index()) {
                throw std::runtime_error(
                    "amt::series(std::vector<T>) : "
                    "all the elements in the series should have same type");
            }
            ++i;
        }
    }

    template <typename T> inline constexpr void push_back(T &&val) {
        m_data.push_back(std::move(val));

        if (m_data[0].index() != back().index()) {
            throw std::runtime_error(
                "amt::series::push_back(T&&) : "
                "all the elements in the series should have same type");
        }
    }

    template <typename T, typename... Args>
    inline void emplace_back(Args &&... args) {
        push_back(std::move(T(std::forward<Args>(args)...)));
    }

    [[nodiscard]] inline constexpr size_type size() const noexcept {
        return m_data.size();
    }

    [[nodiscard]] inline constexpr bool empty() const noexcept {
        return m_data.empty();
    }

    [[nodiscard]] inline constexpr reference operator[](size_type k) {
        return m_data[k];
    }

    [[nodiscard]] inline constexpr const_reference
    operator[](size_type k) const {
        return m_data[k];
    }

    [[nodiscard]] inline constexpr reference at(size_type k) {
        return m_data.at(k);
    }

    [[nodiscard]] inline constexpr const_reference at(size_type k) const {
        return m_data.at(k);
    }

    [[nodiscard]] inline constexpr reference back() { return m_data.back(); }

    [[nodiscard]] inline constexpr const_reference back() const {
        return m_data.back();
    }

    [[nodiscard]] inline constexpr pointer data() noexcept {
        return m_data.data();
    }

    [[nodiscard]] inline constexpr const_pointer data() const noexcept {
        return m_data.data();
    }

    [[nodiscard]] inline constexpr iterator begin() noexcept {
        return m_data.begin();
    }

    [[nodiscard]] inline constexpr iterator end() noexcept {
        return m_data.end();
    }

    [[nodiscard]] inline constexpr const_iterator begin() const noexcept {
        return m_data.begin();
    }

    [[nodiscard]] inline constexpr const_iterator end() const noexcept {
        return m_data.end();
    }

    [[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept {
        return m_data.rbegin();
    }

    [[nodiscard]] inline constexpr reverse_iterator rend() noexcept {
        return m_data.rend();
    }

    [[nodiscard]] inline constexpr const_reverse_iterator
    rbegin() const noexcept {
        return m_data.rbegin();
    }

    [[nodiscard]] inline constexpr const_reverse_iterator
    rend() const noexcept {
        return m_data.rend();
    }

    [[nodiscard]] inline iterator erase(iterator pos) {
        return m_data.erase(pos);
    }

    inline void erase(typename iterator::difference_type pos) {
        m_data.erase(m_data.begin() + pos);
    }

    [[nodiscard]] inline iterator erase(const_iterator pos) {
        return m_data.erase(pos);
    }

    [[nodiscard]] inline iterator erase(iterator first, iterator last) {
        return m_data.erase(first, last);
    }

    [[nodiscard]] inline iterator erase(const_iterator first,
                                        const_iterator last) {
        return m_data.erase(first, last);
    }

    inline void clear() { m_data.clear(); }

    inline void resize(size_type sz) { m_data.resize(sz); }

    inline constexpr void reserve(size_type cap) { m_data.reserve(cap); }

    template <typename T> inline void resize(size_type sz, T &&val) {
        if (!empty() && !back().empty()) {
            bool flag = false;

            if constexpr (std::is_same_v<T, value_type>) {
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

    [[nodiscard]] inline constexpr bool is_string() const noexcept {
        return !empty() && back().is_string();
    }

    [[nodiscard]] inline constexpr bool is_bool() const noexcept {
        return !empty() && back().is_bool();
    }

    [[nodiscard]] inline constexpr bool is_char() const noexcept {
        return !empty() && back().is_char();
    }

    [[nodiscard]] inline constexpr bool is_float() const noexcept {
        return !empty() && back().is_float();
    }

    [[nodiscard]] inline constexpr bool is_double() const noexcept {
        return !empty() && back().is_double();
    }

    [[nodiscard]] inline constexpr bool is_integer() const noexcept {
        return !empty() && back().is_integer();
    }

    template <typename T>
    [[nodiscard]] inline constexpr bool is_type() const noexcept {
        return !empty() && back().template is_type<T>();
    }

    template <typename T>
    [[nodiscard]] inline constexpr auto type_index() const noexcept {
        return value_type::template type_index<T>();
    }

    [[nodiscard]] inline constexpr auto check_types() const noexcept {
        for (auto const &el : *this) {
            if ( ( m_data[0].index() != el.index() ) || el.empty() ) {
                return false;
            }
        }
        return true;
    }

    template <typename T>
    [[nodiscard]] inline constexpr view<series, true>
    operator()(basic_slice<T> s) const noexcept {
        return {*this, std::move(s)};
    }

    template <typename T>
    [[nodiscard]] inline constexpr view<series, false>
    operator()(basic_slice<T> s) noexcept {
        return {*this, std::move(s)};
    }

    template <typename T>
    [[nodiscard]] inline constexpr view<series, true>
    operator()() const noexcept {
        return {*this};
    }

    template <typename T>
    [[nodiscard]] inline constexpr view<series, false> operator()() noexcept {
        return {*this};
    }

  private:
    base_type m_data;
};

template <typename... Ts> std::string type_to_string(series<Ts...> const &s) {
    return s.empty() ? "Empty" : type_to_string(s[0]);
}


template <typename T>
[[nodiscard]] inline constexpr bool
holds_alternative(Series auto &&s) noexcept {
    using type = std::decay_t<decltype(s)>;
    using storage_type = typename type::value_type;
    return storage_type::template type_index<T>() < storage_type::size;
}

} // namespace amt

#endif // AMT_SERIES_HPP
