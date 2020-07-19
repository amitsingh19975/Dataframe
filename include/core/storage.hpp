#if !defined(AMT_CORE_STORAGE_HPP)
#define AMT_CORE_STORAGE_HPP

#include <core/concepts.hpp>
#include <core/type_to_string.hpp>
#include <core/visit.hpp>
#include <iostream>

namespace amt {

template <typename... Ts> struct storage {

    using index_type = std::size_t;
    using size_type = std::size_t;

    using aligned_type =
        std::aligned_union<0, std::size_t, std::string, double, Ts...>;
    using base_type = typename aligned_type::type;

    using type_list = core::normalized_list_t<
        std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::uint8_t,
        std::uint16_t, std::uint32_t, std::uint64_t, std::size_t,
        std::ptrdiff_t, std::string, float, double, bool, Ts...>;

    static constexpr auto const size = boost::mp11::mp_size<type_list>::value;
    static constexpr auto const npos = std::numeric_limits<index_type>::max();
    static constexpr auto const is_packed = false; // core::is_packed_v<Ts...>;
    static constexpr auto const alignment = aligned_type::alignment_value;
    static constexpr auto const byte_size =
        std::numeric_limits<unsigned char>::digits;

    constexpr storage() noexcept = default;
    constexpr storage(storage const &other)
        : m_max_number_of_elem(other.m_max_number_of_elem) {
        if constexpr (!is_packed) {
            visit(other, [this](auto &&val) { this->init_data(val); });
        } else {
            visit(other, [this, &other](auto &&val) {
                using type = std::decay_t<decltype(val)>;
                auto *ptr = &other.cast<type>();
                for (auto i = 0ul; i < other.elem_stored(); ++i) {
                    this->init_data(ptr[i]);
                }
            });
        }
    }
    constexpr storage(storage &&other) noexcept
        : m_max_number_of_elem(other.m_max_number_of_elem) {
        if constexpr (!is_packed) {
            visit(other,
                  [this](auto &&val) { this->init_data(std::move(val)); });
        } else {
            visit(other, [this, &other](auto &&val) {
                using type = std::decay_t<decltype(val)>;
                auto *ptr = &other.cast<type>();
                for (auto i = 0ul; i < other.elem_stored(); ++i) {
                    this->init_data(std::move(ptr[i]));
                }
            });
        }
        other.reset();
    }

    constexpr storage &operator=(storage &&other) noexcept {
        reset();
        m_max_number_of_elem = other.m_max_number_of_elem;

        if constexpr (!is_packed) {
            visit(other,
                  [this](auto &&val) { this->init_data(std::move(val)); });
        } else {
            visit(other, [this, &other](auto &&val) {
                using type = std::decay_t<decltype(val)>;
                auto *ptr = &other.cast<type>();
                for (auto i = 0ul; i < other.elem_stored(); ++i) {
                    this->init_data(std::move(ptr[i]));
                }
            });
        }
        return *this;
    }

    constexpr storage &operator=(storage const &other) {
        reset();
        m_max_number_of_elem = other.m_max_number_of_elem;

        if constexpr (!is_packed) {
            visit(other, [this](auto &&val) { this->init_data(val); });
        } else {
            visit(other, [this, &other](auto &&val) {
                using type = std::decay_t<decltype(val)>;
                auto *ptr = &other.cast<type>();
                for (auto i = 0ul; i < other.elem_stored(); ++i) {
                    this->init_data(ptr[i]);
                }
            });
        }
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

    // [[nodiscard]] inline constexpr char max_number_of_elem() const noexcept {
    //     if constexpr( is_packed ){
    //         return m_max_number_of_elem;
    //     }else{
    //         return 1;
    //     }
    // }

    // [[nodiscard]] inline constexpr size_type elem_stored() const noexcept {
    //     return static_cast<size_type>(m_pos);
    // }

    [[nodiscard]] inline constexpr bool full() const noexcept {
        if constexpr (is_packed) {
            return m_max_number_of_elem == m_pos;
        } else {
            return !empty();
        }
    }

    // template<typename T>
    // inline constexpr void push_back(T&& val) {
    //     if constexpr( is_packed ){
    //         if (!empty() && type_index<T>() != index()) {
    //             throw std::runtime_error("amt::storage::push_back(T&&) : "
    //                                     "all the types should be of same
    //                                     type");
    //         }
    //     }
    //     init_data(std::move(val));
    // }

    // template<typename T>
    // [[nodiscard]] inline constexpr auto& at(size_type k) {
    //     if( empty() ){
    //         throw std::runtime_error("amt::storage::at(size_type) : "
    //                                  "storage is empty");
    //     }

    //     if constexpr( is_packed ){
    //         using curr_type = std::decay_t<T>;
    //         using ntype = core::norm_type_t<curr_type>;

    //         ntype* ptr = &cast<T>();
    //         if( k >= static_cast<size_type>( m_pos ) ){
    //             throw std::out_of_range("amt::storage::at(size_type) : "
    //                                     "out of bound");
    //         }
    //         return ptr[k];
    //     }else{
    //         return cast<T>();
    //     }
    // }

    // template<typename T>
    // [[nodiscard]] inline constexpr auto const& at(size_type k) const {
    //     if( empty() ){
    //         throw std::runtime_error("amt::storage::at(size_type) : "
    //                                  "storage is empty");
    //     }

    //     if constexpr( is_packed ){
    //         using curr_type = std::decay_t<T>;
    //         using ntype = core::norm_type_t<curr_type>;

    //         ntype* ptr = &cast<T>();
    //         if( k >= static_cast<size_type>( m_pos ) ){
    //             throw std::out_of_range("amt::storage::at(size_type) : "
    //                                     "out of bound");
    //         }
    //         return ptr[k];
    //     }else{
    //         return cast<T>();
    //     }
    // }

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
        if constexpr (is_packed) {
            if (m_max_number_of_elem == -1) {
                m_max_number_of_elem = alignment / alignof(T);
            } else {
                if (full()) {
                    throw std::length_error(
                        "amt::storage::allocate(T&&) : "
                        "storage is full, can not allocate more");
                }
            }
            auto *ptr = &cast<type>();
            new (ptr + m_pos++) type(std::move(val));
        } else {
            new (&m_data) type(std::move(val));
        }
    }

    template <typename T> inline constexpr auto deallocate(T &val) { val.~T(); }

    inline constexpr auto reset() {
        if (!empty()) {
            visit(*this, [this](auto &&val) { this->deallocate(val); });
            m_idx = npos;
            m_max_number_of_elem = -1;
            m_pos = 0;
        }
    }

  private:
    base_type m_data;
    index_type m_idx{npos};
    char m_max_number_of_elem{-1};
    unsigned char m_pos{0};
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
