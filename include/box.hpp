#if !defined(AMT_BOX_HPP)
#define AMT_BOX_HPP

#include <core/result.hpp>
#include <core/storage.hpp>
#include <sstream>

namespace amt {

template <typename... Ts> struct box {
    using base_type = core::storage<Ts...>;
    using index_type = typename base_type::index_type;
    using size_type = typename base_type::size_type;
    using stored_types = typename base_type::stored_types;

    static constexpr auto const npos = base_type::npos;

    constexpr box() = default;
    constexpr box(box const &other) : m_data(other.m_data) {}
    constexpr box(box &&other) : m_data(std::move(other.m_data)) {}
    constexpr box &operator=(box const &other) = default;
    constexpr box &operator=(box &&other) = default;
    constexpr ~box() = default;

    template <typename U>
    requires(not Box<U> && not Result<U>) constexpr box(U &&data)
        : m_data(std::forward<U>(data)) {}

    template <typename U> constexpr box(result<U> const &data){
        if (!data) {
            throw std::runtime_error(data.what());
        }
        auto temp = box(*data);
        swap(temp,*this);
    }

    template <typename U>
    constexpr box(result<U> &&data) : box(std::move(*data)) {
        if (!data) {
            throw std::runtime_error(data.what());
        }
        auto temp = box(std::move(*data));
        swap(temp,*this);
    }

    [[nodiscard]] constexpr index_type index() const noexcept {
        return m_data.index();
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return m_data.empty();
    }

    template <typename U>
    [[nodiscard]] static constexpr index_type type_index() noexcept {
        return base_type::template type_index<U>();
    }

    template <typename U>
    [[nodiscard]] constexpr bool valid_index() const noexcept {
        return m_data.template valid_index<U>();
    }

    template <typename U>[[nodiscard]] constexpr auto const &as() const {
        return m_data.template as<U>();
    }

    template <typename U>[[nodiscard]] constexpr auto &as() {
        return m_data.template as<U>();
    }

    template <typename U>[[nodiscard]] constexpr bool is_type() const noexcept {
        return index() == type_index<U>();
    }

    [[nodiscard]] constexpr bool is_char() const noexcept {
        return is_type<char>() || is_type<unsigned char>();
    }

#define AMT_DEFINE_DEFAULT_TYPES(TYPE, MESS, NAME)                             \
    [[nodiscard]] constexpr bool is_##NAME() const noexcept {                  \
        return is_type<TYPE>();                                                \
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

    [[nodiscard]] std::string debug() const {
        std::stringstream s;
        visit(*this, [&s](auto const &val) {
            s << "[ " << core::type_to_string(val) << " ]: " << val;
        });
        return s.str();
    }

    friend std::ostream &operator<<(std::ostream &os, box const &b) {
        visit(b, [&os](auto const &val) { os << val; });
        return os;
    }

    [[nodiscard]] constexpr base_type &data() noexcept { return m_data; }

    [[nodiscard]] constexpr base_type const &data() const noexcept {
        return m_data;
    }

    friend void swap(box& lhs, box& rhs){
        std::swap(lhs.m_data, rhs.m_data);
    }

  private:
    base_type m_data;
};

[[nodiscard]] inline std::string type_to_string(Box auto const &b) {
    std::string temp = "None";
    visit(b, [&temp](auto const &val) { temp = core::type_to_string(val); });
    return temp;
}

} // namespace amt

#endif // AMT_BOX_HPP
