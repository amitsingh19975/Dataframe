#if !defined(AMT_CORE_STORAGE_HPP)
#define AMT_CORE_STORAGE_HPP

#include <core/concepts.hpp>
// #include <core/logger.hpp>
#include <boost/mp11/mpl_list.hpp>
#include <core/type_to_string.hpp>
#include <core/visit.hpp>

namespace amt {

template <typename... Ts> struct storage {

    using index_type = std::size_t;
    using size_type = std::size_t;

    using base_type =
        std::aligned_union_t<sizeof(std::size_t), std::string, double, Ts...>;

    using type_list = boost::mp11::mp_list<
        std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::uint8_t,
        std::uint16_t, std::uint32_t, std::uint64_t, std::size_t,
        std::ptrdiff_t, std::string, float, double, bool, Ts...>;

    static constexpr auto const size = boost::mp11::mp_size<type_list>::value;
    static constexpr auto const npos = std::numeric_limits<index_type>::max();

    constexpr storage() noexcept = default;
    constexpr storage(storage const &other) {
        visit(other, [this](auto &&val) { this->init_data(val); });
    }
    constexpr storage(storage &&other) noexcept {
        visit(other, [this](auto &&val) { this->init_data(std::move(val)); });
        other.reset();
    }

    constexpr storage &operator=(storage &&other) noexcept {
        reset();
        visit(other, [this](auto &&val) { this->init_data(std::move(val)); });
        return *this;
    }

    constexpr storage &operator=(storage const &other) {
        reset();
        visit(other, [this](auto &&val) { this->init_data(val); });
        return *this;
    }

    template <typename T> constexpr storage(T val) {
        init_data(std::move(val));
    }

    template <typename T> constexpr storage &operator=(T val) {
        reset();
        init_data(std::move(val));
        return *this;
    }

    [[nodiscard]] inline constexpr index_type index() const noexcept {
        return m_idx;
    }

    [[nodiscard]] inline constexpr bool empty() const noexcept {
        return m_idx >= size;
    }

    template <typename T>
    [[nodiscard]] inline static constexpr index_type type_index() noexcept {
        using curr_type = std::decay_t<T>;
        using ntype = core::norm_type_t<curr_type>;
        using type_pos = boost::mp11::mp_find<type_list, ntype>;
        return type_pos::value;
    }

    template <typename T>
    [[nodiscard]] inline constexpr bool valid_index() const noexcept {
        return type_index<T>() < size;
    }

    template <typename To>
    [[nodiscard]] inline constexpr auto const &cast() const {
        if (type_index<To>() != index()) {
            throw std::bad_cast();
        }
        using curr_type = std::decay_t<To>;
        using ntype = core::norm_type_t<curr_type>;
        return *static_cast<ntype const *>(static_cast<void const *>(&m_data));
    }

    template <typename To>[[nodiscard]] inline constexpr auto &cast() {
        if (type_index<To>() != index()) {
            throw std::bad_cast();
        }
        using curr_type = std::decay_t<To>;
        using ntype = core::norm_type_t<curr_type>;
        return *static_cast<ntype *>(static_cast<void *>(&m_data));
    }

    template <typename T>
    [[nodiscard]] inline constexpr bool is_type() const noexcept {
        return index() == type_index<T>();
    }

    [[nodiscard]] inline constexpr bool is_string() const noexcept {
        return is_type<std::string>();
    }

    [[nodiscard]] inline constexpr bool is_bool() const noexcept {
        return is_type<bool>();
    }

    [[nodiscard]] inline constexpr bool is_char() const noexcept {
        return is_type<char>() || is_type<unsigned char>();
    }

    [[nodiscard]] inline constexpr bool is_float() const noexcept {
        return is_type<float>();
    }

    [[nodiscard]] inline constexpr bool is_double() const noexcept {
        return is_type<double>();
    }

    [[nodiscard]] inline constexpr bool is_integer() const noexcept {
        return is_type<std::int16_t>() || is_type<std::int8_t>() ||
               is_type<std::int32_t>() || is_type<std::int64_t>() ||
               is_type<std::uint8_t>() || is_type<std::uint16_t>() ||
               is_type<std::uint32_t>() || is_type<std::uint64_t>() ||
               is_type<std::size_t>() || is_type<std::ptrdiff_t>();
    }

    ~storage() { reset(); }

  private:
    template <typename T> inline constexpr void init_data(T &&val) {
        using curr_type = std::decay_t<T>;
        using ntype = core::norm_type_t<curr_type>;
        if (!valid_index<ntype>()) {
            throw std::runtime_error("amt::storage::init_data(T&&) : "
                                     "type is not present in the types");
        }
        m_idx = type_index<ntype>();
        this->allocate(std::move(ntype(std::move(val))));
    }

    template <typename T> inline constexpr void allocate(T &&val) {
        using type = std::decay_t<T>;
        new (&m_data) type(std::move(val));
    }

    template <typename T> inline constexpr auto deallocate(T &val) { val.~T(); }

    inline constexpr auto reset() {
        if (!empty()) {
            visit(*this, [this](auto &&val) { this->deallocate(val); });
            m_idx = npos;
        }
    }

  private:
    base_type m_data;
    index_type m_idx{npos};
};

template <typename... Ts> std::string type_to_string(storage<Ts...> const &s) {
    std::string temp;

    visit(s, [&temp](auto &&val) {
        using type = std::decay_t<decltype(val)>;
        temp = core::type_to_string<type>();
    });

    return temp;
}

template <typename T>
[[nodiscard]] inline constexpr bool
holds_alternative(Storage auto &&s) noexcept {
    return s.template valid_index<T>();
}

} // namespace amt

#endif // AMT_CORE_STORAGE_HPP
