#if !defined(AMT_DATAFRAME_BOX_HPP)
#define AMT_DATAFRAME_BOX_HPP

#include <box_utils.hpp>
#include <core/macro.hpp>
#include <ostream>
#include <string>

namespace amt {
template <typename... Ts> struct basic_box {
    using type_list =
        std::tuple<std::monostate, char, short int, int, long int,
                   long long int, unsigned char, unsigned short int,
                   unsigned int, unsigned long int, unsigned long long int,
                   bool, float, double, std::string, Ts...>;
    using base_type = varaint_from_tuple_t<type_list>;
    using reference = base_type &;
    using const_reference = base_type const &;
    using pointer = base_type *;
    using const_pointer = base_type const *;
    using size_type = std::size_t;

    static_assert((std::is_copy_constructible_v<Ts> && ... && true),
                  "amt::basic_box : types must be copy-constructible");

    static_assert((std::is_move_constructible_v<Ts> && ... && true),
                  "amt::basic_box : types must be move-constructible");

    constexpr basic_box() noexcept = default;
    constexpr basic_box(basic_box const &other) : m_data(other.m_data) {
        if (m_index != 0u && other.m_index != m_index) {
            throw std::runtime_error(ERR_CSTR(
                "amt::basic_box(basic_box const& other) : type mismatch"));
        }

        m_index = other.m_index;
    }

    constexpr basic_box(basic_box &&other) : m_data(std::move(other.m_data)) {
        if (m_index != 0u && other.m_index != m_index) {
            throw std::runtime_error(
                ERR_CSTR("amt::basic_box(basic_box && other) : type mismatch"));
        }

        m_index = other.m_index;
    }

    constexpr basic_box &operator=(basic_box const &other) = default;
    constexpr basic_box &operator=(basic_box &&other) = default;
    constexpr ~basic_box() = default;

    constexpr basic_box(DType auto dtype) noexcept
        : m_index(get<basic_box>(dtype)) {
              construct(dtype_result_t<std::monostate, decltype(dtype)>{});
          }

    template <typename U>
    requires(has_type_v<U, basic_box> &&
             !is_std_string_v<U>) constexpr basic_box(U u, DType auto dtype)
        : m_data(
              std::in_place_index<
                  type_index_v<dtype_result_t<U, decltype(dtype)>, basic_box>>,
              std::move(u)),
          m_index(get<basic_box>(dtype)) {
        if (!is_valid()) {
            throw std::runtime_error(
                ERR_CSTR("amt::basic_box(U u, DType auto) : type mismatch"));
        }
    }

    template <typename U>
    requires(has_type_v<U, basic_box> &&
             !is_std_string_v<U>) constexpr basic_box(U u)
        : m_data(std::in_place_index<type_index_v<U, basic_box>>,
                 std::move(u)) {}

    template <typename U>
    requires(has_type_v<U, basic_box> &&
             !is_std_string_v<U>) constexpr basic_box &
    operator=(U u) {
        basic_box temp(::amt::dtype<>{m_index});
        temp.construct(std::move(u));
        std::swap(*this, temp);
        return *this;
    }

    constexpr basic_box(std::string_view u, DType auto dtype)
        : m_data(std::string(u)), m_index(get<basic_box>(dtype)) {
        if (!is_valid()) {
            throw std::runtime_error(
                ERR_CSTR("amt::basic_box(std::string_view u, DType auto) : "
                         "type mismatch"));
        }
    }

    constexpr basic_box(std::string_view u) : m_data(std::string(u)) {}

    constexpr basic_box &operator=(std::string_view u) {
        basic_box temp(::amt::dtype<std::string>{});
        temp.construct(std::string(u));
        std::swap(*this, temp);
        return *this;
    }

    constexpr size_type index() const noexcept { return m_data.index(); }

    constexpr reference base() noexcept { return m_data; }

    constexpr const_reference base() const noexcept { return m_data; }

    constexpr bool empty() const noexcept { return index() == 0u; }

    template <typename T, typename... Args>
    constexpr T &emplace(Args &&... args) {
        auto ret = m_data.template emplace<T>(std::forward<Args>(args)...);
        if (!is_valid()) {
            throw std::runtime_error(
                ERR_CSTR("amt::emplace<T>(Args&&...) : type mismatch"));
        }
        return ret;
    }

    template <std::size_t I, typename... Args>
    constexpr box_alternative_t<I, basic_box> &emplace(Args &&... args) {
        auto ret = m_data.template emplace<I>(std::forward<Args>(args)...);
        if (!is_valid()) {
            throw std::runtime_error(
                ERR_CSTR("amt::emplace<I>(Args&&...) : type mismatch"));
        }
        return ret;
    }

    constexpr bool operator<(basic_box const &rhs) const {
        if (index() != rhs.index())
            return false;
        return visit(*this, [&rhs]<typename T>(T const &v) {
            if constexpr (HasLess<T, T>) {
                return v < get<T>(rhs);
            } else {
                return false;
            }
        });
    }

    constexpr bool operator==(basic_box const &rhs) const {
        if (index() != rhs.index())
            return false;
        return visit(*this, [&rhs]<typename T>(T const &v) {
            if constexpr (HasLess<T, T>) {
                return v == get<T>(rhs);
            } else {
                return false;
            }
        });
    }

    constexpr bool operator>(basic_box const &rhs) const { return rhs < *this; }

    constexpr bool operator!=(basic_box const &rhs) const {
        return !(*this == rhs);
    }

    constexpr bool operator>=(basic_box const &rhs) const {
        return (*this > rhs) || (*this == rhs);
    }

    constexpr bool operator<=(basic_box const &rhs) const {
        return (rhs >= *this);
    }

    template <typename U>
    requires has_type_v<U, basic_box> constexpr bool
    operator<(U const &rhs) const {
        return *this < basic_box(rhs);
    }

    template <typename U>
    requires has_type_v<U, basic_box> constexpr bool
    operator>(U const &rhs) const {
        return *this > basic_box(rhs);
    }

    template <typename U>
    requires has_type_v<U, basic_box> constexpr bool
    operator<=(U const &rhs) const {
        return *this <= basic_box(rhs);
    }

    template <typename U>
    requires has_type_v<U, basic_box> constexpr bool
    operator>=(U const &rhs) const {
        return *this >= basic_box(rhs);
    }

    template <typename U>
    requires has_type_v<U, basic_box> constexpr bool
    operator==(U const &rhs) const {
        return *this == basic_box(rhs);
    }

    template <typename U>
    requires has_type_v<U, basic_box> constexpr bool
    operator!=(U const &rhs) const {
        return *this != basic_box(rhs);
    }

    constexpr basic_box &operator++() {
        basic_box res;
        visit(*this, [&res]<typename T>(T const &val) {
            if constexpr (HasPreInc<T>) {
                auto temp = val;
                res = ++temp;
            }
        });
        if (res.empty()) {
            throw std::runtime_error(ERR_CSTR(
                "amt::basic_box::operator++() : the operator not found"));
        }
        (*this) = std::move(res);
        return *this;
    }

    constexpr basic_box operator++(int) {
        auto temp = *this ++(*this);
        return temp;
    }

    constexpr basic_box &operator--() {
        basic_box res;
        visit(*this, [&res]<typename T>(T const &val) {
            if constexpr (HasPreInc<T>) {
                auto temp = val;
                res = --temp;
            }
        });
        if (res.empty()) {
            throw std::runtime_error(ERR_CSTR(
                "amt::basic_box::operator--() : the operator not found"));
        }
        (*this) = std::move(res);
        return *this;
    }

    constexpr basic_box operator--(int) {
        auto temp = *this;
        --(*this);
        return temp;
    }

    template <typename U>
    requires has_type_v<U, basic_box> constexpr operator U() const {
        if (!is<U>(*this)) {
            throw std::runtime_error(
                ERR_CSTR("amt::basic_box::operator U() : invalid conversion"));
        }
        return std::get<U>(m_data);
    }

    constexpr void reset_dtype() noexcept { m_index = 0u; }

    constexpr void dtype(DType auto dtype) {
        m_index = get<basic_box>(dtype);
        if (!is_valid()) {
            throw std::runtime_error(
                ERR_CSTR("amt::basic_box::dtype(DType auto) : type mismatch"));
        }
    }

    constexpr auto dtype() const noexcept { return ::amt::dtype<>(m_index); }

    constexpr bool is_valid() const noexcept {
        return (m_index == 0) || (m_data.index() == m_index);
    }

  private:
    template <typename T> constexpr void construct(T &&val) {
        if( m_index == 0u ){
            m_data = std::move(val);
        }else{
            tuple_for<type_list>([&val, this]<typename I>(I) {
                using element_type =
                    std::decay_t<std::tuple_element_t<I::value, type_list>>;
                using t_type = std::decay_t<T>;
                constexpr auto idx = type_index_v<element_type, basic_box>;
                if constexpr (std::is_same_v<t_type, element_type>) {
                    if (m_index == idx) {
                        m_data = std::move(val);
                        return;
                    }
                }
                if constexpr (std::is_convertible_v<t_type, element_type>) {                                                                
                    if (m_index == idx) {
                        m_data = std::move(val);
                    }
                } else if constexpr (std::is_constructible_v<element_type,
                                                                t_type>) {
                    if (m_index == idx) {
                        m_data = element_type(std::move(val));
                    }
                }
            });
            
            if (empty()) {
                throw std::runtime_error(
                    ERR_CSTR("amt::basic_box::construct(T&&) : type mismatch"));
            }
        }

    }
    
    constexpr void construct(std::monostate) {
        tuple_for<type_list>([this]<typename I>(I) {
            using element_type =
                    std::decay_t<std::tuple_element_t<I::value, type_list>>;
            constexpr auto idx = type_index_v<element_type, basic_box>;
            if (m_index == idx) {
                m_data = element_type{};
                return;
            }
        });
    }

  private:
    base_type m_data;
    size_type m_index{};
};

inline static constexpr std::size_t box_npos = static_cast<std::size_t>(-1);

using box = basic_box<>;

} // namespace amt

namespace std {
template <typename... Ts> struct hash<::amt::basic_box<Ts...>> {
    std::size_t operator()(::amt::basic_box<Ts...> const &b) const noexcept {
        return amt::visit(b,
                          []<typename T>(T const &v) { return hash<T>{}(v); });
    }
};
} // namespace std

std::ostream &operator<<(std::ostream &os, amt::Box auto const &b) {
    amt::visit(b, [&os](auto &&v) {
        using value_type = std::decay_t<decltype(v)>;
        if constexpr (!std::is_same_v<value_type, std::monostate>)
            os << ::amt::beautify(v);
        else
            os << "none";
    });
    return os;
}

template <amt::Box BoxType>
constexpr BoxType operator+(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator+(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasPlus<ltype, rtype>) {
                res = (lval + rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator+(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator-(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator-(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasMinus<ltype, rtype>) {
                res = (lval - rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator-(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator*(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator*(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasMul<ltype, rtype>) {
                res = (lval * rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator*(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator/(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator/(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasDiv<ltype, rtype>) {
                res = (lval / rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator/(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator+=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs + rhs;
    return lhs;
}

template <amt::Box BoxType>
constexpr BoxType operator-=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs - rhs;
    return lhs;
}

template <amt::Box BoxType>
constexpr BoxType operator*=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs * rhs;
    return lhs;
}

template <amt::Box BoxType>
constexpr BoxType operator/=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs / rhs;
    return lhs;
}

template <amt::Box BoxType>
constexpr BoxType operator%(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator%(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasMod<ltype, rtype>) {
                res = (lval % rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator%(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator%=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs % rhs;
    return lhs;
}

template <amt::Box BoxType>
constexpr BoxType operator&(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator&(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasBitwiseAND<ltype, rtype>) {
                res = (lval & rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator&(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator|(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator|(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasBitwiseOR<ltype, rtype>) {
                res = (lval | rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator|(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator^(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator^(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasBitwiseXOR<ltype, rtype>) {
                res = (lval ^ rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator^(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator&=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs & rhs;
    return lhs;
}

template <amt::Box BoxType>
constexpr BoxType operator|=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs | rhs;
    return lhs;
}

template <amt::Box BoxType>
constexpr BoxType operator^=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

template <amt::Box BoxType>
constexpr BoxType operator&&(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator&&(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasLogicalAND<ltype, rtype> &&
                          !std::is_floating_point_v<ltype>) {
                res = (lval && rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator&&(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator||(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator||(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasLogicalOR<ltype, rtype> &&
                          !std::is_floating_point_v<ltype>) {
                res = (lval && rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator||(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator>>(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator>>(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasRightShift<ltype, rtype>) {
                res = (lval >> rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator>>(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator<<(BoxType const &lhs, BoxType const &rhs) {
    BoxType res;

    if (lhs.index() != rhs.index())
        throw std::runtime_error(ERR_CSTR("operator<<(BoxType const&, "
                                          "BoxType const&) : type mismatch"));

    apply_binary_operator(lhs, rhs, [&](auto &&lval, auto &&rval) {
        using ltype = std::decay_t<decltype(lval)>;
        using rtype = std::decay_t<decltype(rval)>;

        if (lhs.index() == rhs.index()) {
            if constexpr (amt::HasLeftShift<ltype, rtype>) {
                res = (lval << rval);
            }
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator<<(BoxType const&, "
                     "BoxType const&) : the operator not found"));

    return res;
}

template <amt::Box BoxType>
constexpr BoxType operator<<=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs << rhs;
    return lhs;
}

template <amt::Box BoxType>
constexpr BoxType operator>>=(BoxType &lhs, BoxType const &rhs) {
    lhs = lhs >> rhs;
    return lhs;
}

template <amt::Box BoxType> constexpr BoxType operator!(BoxType const &lhs) {
    BoxType res;

    apply_unary_operator(lhs, [&](auto &&lval) {
        using ltype = std::decay_t<decltype(lval)>;

        if constexpr (amt::HasUnaryNot<ltype> &&
                      !std::is_floating_point_v<ltype>) {
            res = !lval;
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator!(BoxType const&) : the operator "
                     "is not found"));

    return res;
}

template <amt::Box BoxType> constexpr BoxType operator+(BoxType const &lhs) {
    BoxType res;

    apply_unary_operator(lhs, [&](auto &&lval) {
        using ltype = std::decay_t<decltype(lval)>;

        if constexpr (amt::HasUnaryPlus<ltype>) {
            res = +lval;
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator+(BoxType const&) : the operator "
                     "is not found"));

    return res;
}

template <amt::Box BoxType> constexpr BoxType operator-(BoxType const &lhs) {
    BoxType res;

    apply_unary_operator(lhs, [&](auto &&lval) {
        using ltype = std::decay_t<decltype(lval)>;

        if constexpr (amt::HasUnaryMinus<ltype>) {
            res = -lval;
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator-(BoxType const&) : the operator "
                     "is not found"));

    return res;
}

template <amt::Box BoxType> constexpr BoxType operator~(BoxType const &lhs) {
    BoxType res;

    apply_unary_operator(lhs, [&](auto &&lval) {
        using ltype = std::decay_t<decltype(lval)>;

        if constexpr (amt::HasUnaryNeg<ltype> && !std::is_same_v<ltype, bool>) {
            res = ~lval;
        }
    });

    if (res.empty())
        throw std::runtime_error(
            ERR_CSTR("operator~(BoxType const&) : the operator "
                     "is not found"));

    return res;
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator+(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator+(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator+(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator-(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator-(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator-(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator*(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator*(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator*(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator/(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator/(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator/(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator%(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator%(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator%(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator+=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator+=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator+=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator-=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator-=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator-=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator*=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator*=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator*=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator/=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator/=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator/=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator%=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator%=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator%=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator&(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator&(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator&(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator|(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator|(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator|(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator^(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator^(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator^(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator&=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator&=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator&=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator|=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator|=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator|=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator^=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator^=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator^=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator&&(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator&&(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator&&(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator||(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator||(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator||(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator<<(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator<<(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator<<(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator>>(LHS const &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator>>(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator>>(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator<<=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator<<=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator<<=(temp, rhs);
    }
}

template <typename LHS, typename RHS>
requires(static_cast<bool>(amt::Box<RHS> ^ amt::Box<LHS>)) constexpr auto
operator>>=(LHS &lhs, RHS const &rhs) {
    if constexpr (amt::Box<LHS>) {
        LHS temp(rhs);
        return ::operator>>=(lhs, temp);
    } else {
        RHS temp(lhs);
        return ::operator>>=(temp, rhs);
    }
}

#endif // AMT_DATAFRAME_BOX_HPP
