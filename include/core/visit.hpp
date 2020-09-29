#if !defined(AMT_CORE_VISIT_HPP)
#define AMT_CORE_VISIT_HPP

#include <core/concepts.hpp>

namespace amt::core {

namespace detail {

struct generic_tag {};

template <typename ParamType, typename UnaryFn, std::size_t N = 0>
inline constexpr auto visit_impl(Storage auto &&s, UnaryFn &&fn) {
    using storage_type = std::decay_t<decltype(s)>;
    using type_list = typename storage_type::stored_types;

    constexpr auto const size = storage_type::size;
    constexpr bool const is_generic_type =
        std::is_same_v<ParamType, detail::generic_tag>;

    switch (s.index()) {
    case N + 0: {
        if constexpr (N + 0 < size) {
            using type = at_c<type_list, N + 0>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 1: {
        if constexpr (N + 1 < size) {
            using type = at_c<type_list, N + 1>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 2: {
        if constexpr (N + 2 < size) {
            using type = at_c<type_list, N + 2>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 3: {
        if constexpr (N + 3 < size) {
            using type = at_c<type_list, N + 3>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 4: {
        if constexpr (N + 4 < size) {
            using type = at_c<type_list, N + 4>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 5: {
        if constexpr (N + 5 < size) {
            using type = at_c<type_list, N + 5>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 6: {
        if constexpr (N + 6 < size) {
            using type = at_c<type_list, N + 6>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 7: {
        if constexpr (N + 7 < size) {
            using type = at_c<type_list, N + 7>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 8: {
        if constexpr (N + 8 < size) {
            using type = at_c<type_list, N + 8>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 9: {
        if constexpr (N + 9 < size) {
            using type = at_c<type_list, N + 9>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 10: {
        if constexpr (N + 10 < size) {
            using type = at_c<type_list, N + 10>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 11: {
        if constexpr (N + 11 < size) {
            using type = at_c<type_list, N + 11>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 12: {
        if constexpr (N + 12 < size) {
            using type = at_c<type_list, N + 12>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 13: {
        if constexpr (N + 13 < size) {
            using type = at_c<type_list, N + 13>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    case N + 14: {
        if constexpr (N + 14 < size) {
            using type = at_c<type_list, N + 14>;
            if constexpr ((is_generic_type ||
                           std::is_same_v<ParamType, type>)) {
                decltype(auto) data = s.template as<type>();
                fn(data);
            }
        }
        return;
    }
    default: {
        if constexpr (N + 15 < size) {
            return visit_impl<ParamType, UnaryFn, N + 15>(
                std::forward<decltype(s)>(s), std::move(fn));
        }
    }
    }
}

} // namespace detail

template <typename UnaryFn>
inline constexpr auto visit_helper(Storage auto &&s, UnaryFn &&fn) {
    using storage_type = std::decay_t<decltype(s)>;
    using type_list = typename storage_type::stored_types;
    if constexpr (core::is_generic_unary_v<type_list, UnaryFn>) {
        detail::visit_impl<detail::generic_tag>(std::forward<decltype(s)>(s),
                                                std::forward<UnaryFn>(fn));
    } else {
        using traits = core::function_traits<UnaryFn>;
        using param_type = std::decay_t<typename traits::template arg<0>::type>;
        using nparam_type = core::norm_type_t<param_type>;
        detail::visit_impl<nparam_type>(std::forward<decltype(s)>(s),
                                        std::forward<UnaryFn>(fn));
    }
}

template <typename UnaryFn, typename... Fs>
inline constexpr auto visit(Storage auto &&s, UnaryFn &&fn, Fs &&... fs) {
    using storage_type = decltype(s);
    visit_helper(std::forward<storage_type>(s), std::forward<UnaryFn>(fn));
    (visit_helper(std::forward<storage_type>(s), std::forward<Fs>(fs)), ...);
}

} // namespace amt::core

namespace amt {

template <typename... Fs>
inline constexpr auto visit(Box auto &&s, Fs &&... fs) {
    core::visit(s.data(), std::forward<Fs>(fs)...);
}

} // namespace amt

#endif // AMT_CORE_VISIT_HPP
