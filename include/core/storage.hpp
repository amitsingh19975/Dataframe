#if !defined(AMT_CORE_STORAGE_HPP)
#define AMT_CORE_STORAGE_HPP

#include <core/concepts.hpp>
#include <core/type_to_string.hpp>
#include <core/visit.hpp>
#include <iostream>
#include <new>

namespace amt::core {

template <typename... Ts> struct storage {

    using index_type = std::size_t;
    using size_type = std::size_t;

    using stored_types = core::normalized_list_t<
#define AMT_DEFINE_DEFAULT_TYPES(TYPE, MESS, NAME) TYPE
#define AMT_TYPE_SEPARATOR ,
#include <core/default_types.def>
        , Ts...>;

    using aligned_union_type = std::aligned_union<0,
#define AMT_DEFINE_DEFAULT_TYPES(TYPE, MESS, NAME) TYPE
#define AMT_TYPE_SEPARATOR ,
#include <core/default_types.def>
                                                  , Ts...>;

    using base_type = typename aligned_union_type::type;

    static constexpr auto const size =
        boost::mp11::mp_size<stored_types>::value;
    static constexpr auto const npos = size;
    static constexpr auto const is_packed = false; // core::is_packed_v<Ts...>;
    static constexpr auto const alignment = aligned_union_type::alignment_value;
    static constexpr auto const byte_size =
        std::numeric_limits<unsigned char>::digits;

    constexpr storage() = default;
    constexpr storage(storage const &other) {
        visit(other, [this](auto const &val) { this->allocate(val); });
    }
    constexpr storage(storage &&other) {
        visit(other, [this, &other]<typename T>(T const &val) {
            this->allocate(std::move(val));
            other.deallocate<T>();
        });
    }
    constexpr storage &operator=(storage const &) = default;
    constexpr storage &operator=(storage &&) = default;
    constexpr ~storage() {
        if (!empty()) {
            visit(*this,
                  [this]<typename T>(T const &) { this->deallocate<T>(); });
        }
    };

    template <typename U> requires(not Storage<U>) constexpr storage(U data) {
        allocate(std::move(data));
    }

    [[nodiscard]] constexpr index_type index() const noexcept { return m_idx; }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return m_idx == npos;
    }

    template <typename U>
    [[nodiscard]] static constexpr index_type type_index() noexcept {
        using normalized_t = norm_type_t<U>;
        using type_pos = find_type_t<stored_types, normalized_t>;
        return type_pos::value;
    }

    template <typename U>
    [[nodiscard]] constexpr bool valid_index() const noexcept {
        return type_index<U>() != npos;
    }

    template <typename U>[[nodiscard]] constexpr auto const &as() const {
        constexpr auto pos = type_index<U>();
        if (empty() || m_idx != pos) {
            throw std::bad_cast();
        }
        return *reinterpret_cast<U const *>(&m_data);
    }

    template <typename U>[[nodiscard]] constexpr auto &as() {
        constexpr auto pos = type_index<U>();
        if (empty() || m_idx != pos) {
            throw std::bad_cast();
        }
        return *reinterpret_cast<U *>(&m_data);
    }

  private:
    template <typename U> constexpr void allocate(U &&data) {
        using normalized_t = norm_type_t<U>;
        constexpr auto pos = type_index<normalized_t>();
        static_assert(pos != npos, "amt::core::storage::allocate(U&&): "
                                   "type does not exist in storage");
        m_idx = pos;
        new (&m_data) normalized_t(std::move(data));
    }

    template <typename U> constexpr void deallocate() {
        using normalized_t = norm_type_t<U>;
        auto &temp = *reinterpret_cast<normalized_t *>(&m_data);
        temp.~normalized_t();
        m_idx = npos;
    }

  private:
    base_type m_data;
    index_type m_idx{npos};
    size_type m_align{alignment};
};

} // namespace amt::core

#endif // AMT_CORE_STORAGE_HPP
