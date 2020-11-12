#if !defined(AMT_DATAFRAME_BOX_UTILS_HPP)
#define AMT_DATAFRAME_BOX_UTILS_HPP

#include <dataframe/core/utils.hpp>
#include <dataframe/traits/basic_traits.hpp>
#include <dataframe/traits/box_traits.hpp>
#include <variant>

namespace amt {

template <typename... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <Box V, typename... Fns>
requires(!std::conjunction_v<is_overloaded<Fns>...>) constexpr decltype(auto)
    visit(V &&v, Fns &&... fns) {
    return std::visit(overloaded{std::forward<Fns>(fns)...}, v.base());
}

template <Box V, typename... Ts>
constexpr decltype(auto) visit(V &&v, overloaded<Ts...> overloaded_set) {
    return std::visit(std::move(overloaded_set), v.base());
}

template <typename T> constexpr bool is(Box auto &&b) noexcept {
    return std::holds_alternative<T>(b.base());
}

template <std::size_t I, Box B> constexpr decltype(auto) get(B &&b) {
    return std::get<I>(b.base());
}

template <typename T, Box B> constexpr decltype(auto) get(B &&b) {
    return std::get<T>(b.base());
}

template <std::size_t I, Box B> constexpr decltype(auto) get_if(B *b) {
    return std::get_if<I>(&(b->base()));
}

template <std::size_t I, Box B> constexpr decltype(auto) get_if(B const *b) {
    return std::get_if<I>(&(b->base()));
}

template <typename T, Box B> constexpr decltype(auto) get_if(B *b) {
    return std::get_if<T>(&(b->base()));
}

template <typename T, Box B> constexpr decltype(auto) get_if(B const *b) {
    return std::get_if<T>(&(b->base()));
}

template <Box BoxType, typename Fn>
constexpr void apply_binary_operator(BoxType &&lhs, BoxType &&rhs, Fn &&fn) {
    visit(std::forward<BoxType>(lhs), [&](auto &&lval) {
        visit(std::forward<BoxType>(rhs), [&](auto &&rval) {
            using ltype = std::decay_t<decltype(lval)>;
            using rtype = std::decay_t<decltype(rval)>;

            constexpr bool is_same = std::is_same_v<ltype, rtype>;
            if constexpr (is_same) {
                std::invoke(std::forward<Fn>(fn), lval, rval);
            }
        });
    });
}

template <Box BoxType, typename Fn>
constexpr void apply_unary_operator(BoxType &&b, Fn &&fn) {
    visit(std::forward<BoxType>(b),
          [&](auto &&val) { std::invoke(std::forward<Fn>(fn), val); });
}

template <Box BoxType> std::string type_to_string(BoxType const &b) {
    std::string ret;
    box_type_for<BoxType>([dtype = b.dtype(), &ret]<typename T>(auto idx, T) {
        using el_type = typename T::type;
        if (idx.value == get<BoxType>(dtype)) {
            ret = get_type_name_v<el_type>;
        }
    });
    return ret;
}

constexpr bool is_integer(Box auto const &b) noexcept {
    return is<short int>(b) || is<int>(b) || is<long int>(b) ||
           is<long long int>(b) || is<unsigned short int>(b) ||
           is<unsigned int>(b) || is<unsigned long int>(b) ||
           is<unsigned long long int>(b) || is<std::size_t>(b) ||
           is<std::ptrdiff_t>(b);
}

constexpr bool is_char(Box auto const &b) noexcept {
    return is<char>(b) || is<unsigned char>(b);
}

constexpr bool is_bool(Box auto const &b) noexcept { return is<bool>(b); }

constexpr bool is_floating_point(Box auto const &b) noexcept {
    return is<float>(b) || is<double>(b);
}

constexpr bool is_string(Box auto const &b) noexcept {
    return is<std::string>(b);
}

constexpr bool is_none(Box auto const &b) noexcept {
    return is<std::monostate>(b);
}

constexpr bool is_nan(Box auto const &b) noexcept {
    return visit(b, [](auto &&val) { return val != val; });
}

constexpr bool is_categorical(Box auto const &b) noexcept {
    return !(is_integer(b) || is_floating_point(b));
}

template <Box BoxType> constexpr std::size_t get(DType auto d) noexcept {
    return d.template id<BoxType>();
}

template <typename T> struct dtype<T> {
    using type = T;

    template <Box B> constexpr std::size_t id() const noexcept {
        return type_index_v<T, B>;
    }

    template <typename U>
    constexpr bool operator==(dtype<U> const &) const noexcept {
        return std::is_same_v<T, U>;
    }

    template <typename U>
    constexpr bool operator!=(dtype<U> const &) const noexcept {
        return !std::is_same_v<T, U>;
    }
};

template <> struct dtype<> {

    constexpr dtype() noexcept = default;
    constexpr dtype(dtype const &other) noexcept = default;
    constexpr dtype(dtype &&other) noexcept = default;
    constexpr dtype &operator=(dtype const &other) noexcept = default;
    constexpr dtype &operator=(dtype &&other) noexcept = default;
    constexpr ~dtype() noexcept = default;

    constexpr dtype(std::size_t idx) noexcept : index(idx) {}

    template <Box B> constexpr std::size_t id() const noexcept { return index; }

    constexpr std::size_t id() const noexcept { return index; }

    constexpr bool operator==(dtype const &i) const noexcept = default;
    constexpr bool operator!=(dtype const &i) const noexcept = default;

    constexpr bool operator==(std::size_t i) const noexcept {
        return index == i;
    }
    constexpr bool operator!=(std::size_t i) const noexcept {
        return index != i;
    }

    std::size_t index{};
};

template <Box BoxType> constexpr bool is(DType auto d, BoxType &&b) noexcept {
    auto ti = get<BoxType>(d);
    return b.index() == ti;
}

template <Box BoxType, typename Fn> constexpr void box_type_for(Fn &&fn) {
    using tuple_type = typename std::decay_t<BoxType>::type_list;
    tuple_for<tuple_type>([fn = std::forward<Fn>(fn)]<typename I>(I) {
        using tuple_element = std::tuple_element<I::value, tuple_type>;
        using index_t = type_index<typename tuple_element::type, BoxType>;
        if constexpr (std::is_invocable_v<Fn, index_t, tuple_element>) {
            std::invoke(fn, index_t{}, tuple_element{});
        } else {
            std::invoke(fn, index_t{});
        }
    });
}

} // namespace amt

#endif // AMT_DATAFRAME_BOX_UTILS_HPP
