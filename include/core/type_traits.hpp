#if !defined(AMT_CORE_TYPE_TRAITS_HPP)
#define AMT_CORE_TYPE_TRAITS_HPP

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/mpl_list.hpp>
#include <cstddef>
#include <limits>
#include <string>
#include <type_traits>

namespace amt::core {

template <typename T, typename U>
inline static constexpr bool const has_comp_equal_v = requires(T LHS, U RHS) {
    LHS == RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_comp_less_v = requires(T LHS, U RHS) {
    LHS < RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_comp_greater_v = requires(T LHS, U RHS) {
    LHS > RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_comp_less_equal_v = requires(T LHS,
                                                                    U RHS) {
    LHS <= RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_comp_greater_equal_v = requires(T LHS,
                                                                       U RHS) {
    LHS >= RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_op_plus_v = requires(T LHS, U RHS) {
    LHS + RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_op_minus_v = requires(T LHS, U RHS) {
    LHS - RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_op_mul_v = requires(T LHS, U RHS) {
    LHS *RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_op_div_v = requires(T LHS, U RHS) {
    LHS / RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_op_plus_equal_v = requires(T LHS,
                                                                  U RHS) {
    LHS += RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_op_minus_equal_v = requires(T LHS,
                                                                   U RHS) {
    LHS -= RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_op_mul_equal_v = requires(T LHS, U RHS) {
    LHS *= RHS;
};

template <typename T, typename U>
inline static constexpr bool const has_op_div_equal_v = requires(T LHS, U RHS) {
    LHS /= RHS;
};

template <typename T>
inline static constexpr bool const has_std_to_string_v = requires(T t) {
    std::to_string(t);
};

namespace detail {

template <typename T> constexpr auto get_common_integral_type() noexcept {
#define AMT_BEGIN_DEFAULT_TYPES(MESS)                                          \
    if constexpr (false)                                                       \
        return;
#define AMT_END_DEFAULT_TYPES(MESS) else return std::declval<T>();

#define AMT_DEFINE_DEFAULT_TYPES(TYPE, MESS, NAME)                             \
    else if constexpr ((sizeof(T) == sizeof(TYPE)) and                         \
                       (std::numeric_limits<T>::is_signed ==                   \
                        std::numeric_limits<                                   \
                            TYPE>::is_signed)) return std::declval<TYPE>();

#include <core/default_types.def>
}

} // namespace detail

template <typename T>
using comman_integral_type_t =
    decltype(detail::get_common_integral_type<std::decay_t<T>>());

template <typename T>
struct norm_type
    : std::conditional<
          std::is_integral_v<T>, comman_integral_type_t<T>,
          std::conditional_t<std::is_constructible_v<std::string, T>,
                             std::string, T>> {};

template <> struct norm_type<bool> { using type = bool; };

template <typename T>
using norm_type_t = typename norm_type<std::decay_t<T>>::type;

template <typename> struct always_false : std::false_type {};

template <typename T>
inline static constexpr auto const always_false_v = always_false<T>::value;

template <typename... Ts> struct storage;

template <typename T> struct is_storage : std::false_type {};

template <typename T>
inline static constexpr bool const is_storage_v = is_storage<T>::value;

template <typename... Ts> struct is_storage<storage<Ts...>> : std::true_type {};

} // namespace amt::core

namespace amt {

template <typename... Ts> struct box;

template <typename... Ts> struct series;

template <typename... Ts> struct frame;

template <typename T, bool C> struct view;

template <typename T> struct basic_slice;

struct slice_first;
struct slice_last;
struct slice_stride;

struct in_place_t {};
struct out_place_t {};
struct packed_storage_t {};
inline static constexpr auto const in_place = in_place_t{};
inline static constexpr auto const out_place = out_place_t{};

} // namespace amt

namespace amt {

template <typename T>
struct is_tag : std::disjunction<std::is_same<T, in_place_t>,
                                 std::is_same<T, out_place_t>> {};

template <typename T>
inline static constexpr auto const is_tag_v = is_tag<T>::value;

template <typename T> struct is_slice : std::false_type {};

template <typename T>
inline static constexpr bool const is_slice_v = is_slice<T>::value;

template <typename T> struct is_slice<basic_slice<T>> : std::true_type {};

template <typename T> struct is_box : std::false_type {};

template <typename T>
inline static constexpr bool const is_box_v = is_box<T>::value;

template <typename... Ts> struct is_box<box<Ts...>> : std::true_type {};

template <typename T> struct is_series : std::false_type {};

template <typename T>
inline static constexpr bool const is_series_v = is_series<T>::value;

template <typename... Ts> struct is_series<series<Ts...>> : std::true_type {};

template <typename T> struct is_series_view : std::false_type {};

template <typename T>
inline static constexpr bool const is_series_view_v = is_series_view<T>::value;

template <bool B, typename... Ts>
struct is_series_view<view<series<Ts...>, B>> : std::true_type {};

template <typename T> struct is_frame : std::false_type {};

template <typename T>
inline static constexpr bool const is_frame_v = is_frame<T>::value;

template <typename... Ts> struct is_frame<frame<Ts...>> : std::true_type {};

template <typename T> struct is_frame_view : std::false_type {};

template <typename T>
inline static constexpr bool const is_frame_view_v = is_frame_view<T>::value;

template <bool B, typename... Ts>
struct is_frame_view<view<frame<Ts...>, B>> : std::true_type {};

template <typename T> struct is_view : std::false_type {};

template <typename T>
inline static constexpr bool const is_view_v = is_view<std::decay_t<T>>::value;

template <typename T, bool C> struct is_view<view<T, C>> : std::true_type {};

template <typename... Ts> struct is_proper_slice_args;

template <typename... Ts>
inline static constexpr bool const is_proper_slice_args_v =
    is_proper_slice_args<Ts...>::value;

template <> struct is_proper_slice_args<> : std::true_type {};

template <typename T>
struct is_proper_slice_args<T>
    : std::integral_constant<
          bool, (std::numeric_limits<std::decay_t<T>>::is_integer &&
                 !std::numeric_limits<std::decay_t<T>>::is_signed) ||
                    std::is_same_v<std::decay_t<T>, slice_first> ||
                    std::is_same_v<std::decay_t<T>, slice_last> ||
                    std::is_same_v<std::decay_t<T>, slice_stride>> {};

template <typename... Ts>
struct is_proper_slice_args
    : std::integral_constant<bool, (is_proper_slice_args_v<Ts> || ...) &&
                                       (sizeof...(Ts) <= 3)> {};

template <typename T> struct result_type { using type = T; };

template <typename T, bool B> struct result_type<view<T, B>> {
    using type = T;
};

template <typename T>
using result_type_t = typename result_type<std::decay_t<T>>::type;

} // namespace amt

namespace amt::core {

template <class F> struct function_traits;

template <class R, class... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)> {};

template <class R, class... Args> struct function_traits<R(Args...)> {
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N> struct arg {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
};

template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...)>
    : public function_traits<R(C &, Args...)> {};

template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...) const>
    : public function_traits<R(C &, Args...)> {};

template <class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C &)> {};

template <class F> struct function_traits {
  private:
    using call_type = function_traits<decltype(&F::operator())>;

  public:
    using return_type = typename call_type::return_type;

    static constexpr std::size_t arity = call_type::arity - 1;

    template <std::size_t N> struct arg {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename call_type::template arg<N + 1>::type;
    };
};

template <class F> struct function_traits<F &> : public function_traits<F> {};

template <class F> struct function_traits<F &&> : public function_traits<F> {};

template <typename L, typename Fn, std::size_t I = 0, bool prev = true>
constexpr auto is_generic_unary() noexcept {
    constexpr auto const size = boost::mp11::mp_size<L>::value;
    if constexpr (I >= size) {
        if constexpr (prev) {
            return std::true_type{};
        } else {
            return std::false_type{};
        }
    } else {
        using type = boost::mp11::mp_at_c<L, I>;
        constexpr auto const cond = std::is_invocable_v<Fn, type> ||
                                    std::is_invocable_v<Fn, type &> ||
                                    std::is_invocable_v<Fn, type const> ||
                                    std::is_invocable_v<Fn, type const &>;

        return decltype(is_generic_unary < L, Fn, I + 1, prev && cond > ()){};
    }
}

template <typename L, typename Fn>
inline static constexpr bool is_generic_unary_v =
    decltype(is_generic_unary<L, Fn, 0>())::value;

template <typename L, typename Fn, std::size_t I = 0, bool prev = true>
constexpr auto is_generic_binary() noexcept {
    constexpr auto const size = boost::mp11::mp_size<L>::value;
    if constexpr (I >= size) {
        if constexpr (prev) {
            return std::true_type{};
        } else {
            return std::false_type{};
        }
    } else {
        using type = boost::mp11::mp_at_c<L, I>;
        constexpr auto const cond =
            std::is_invocable_v<Fn, type, type> ||
            std::is_invocable_v<Fn, type, type &> ||
            std::is_invocable_v<Fn, type, type const> ||
            std::is_invocable_v<Fn, type, type const &> ||
            std::is_invocable_v<Fn, type &> ||
            std::is_invocable_v<Fn, type &, type> ||
            std::is_invocable_v<Fn, type &, type &> ||
            std::is_invocable_v<Fn, type &, type const> ||
            std::is_invocable_v<Fn, type &, type const &> ||
            std::is_invocable_v<Fn, type const, type> ||
            std::is_invocable_v<Fn, type const, type &> ||
            std::is_invocable_v<Fn, type const, type const> ||
            std::is_invocable_v<Fn, type const, type const &> ||
            std::is_invocable_v<Fn, type const &, type> ||
            std::is_invocable_v<Fn, type const &, type &> ||
            std::is_invocable_v<Fn, type const &, type const> ||
            std::is_invocable_v<Fn, type const &, type const &>;

        return decltype(is_generic_binary < L, Fn, I + 1, prev && cond > ()){};
    }
}

template <typename T, typename L, typename Fn, std::size_t I = 0,
          bool prev = true>
constexpr auto is_generic_binary() noexcept {
    constexpr auto const size = boost::mp11::mp_size<L>::value;
    if constexpr (I >= size) {
        if constexpr (prev) {
            return std::true_type{};
        } else {
            return std::false_type{};
        }
    } else {
        using type = boost::mp11::mp_at_c<L, I>;
        constexpr auto const cond =
            std::is_invocable_v<Fn, std::decay_t<T>, type> ||
            std::is_invocable_v<Fn, std::decay_t<T>, type &> ||
            std::is_invocable_v<Fn, std::decay_t<T>, type const> ||
            std::is_invocable_v<Fn, std::decay_t<T>, type const &> ||
            std::is_invocable_v<Fn, std::decay_t<T> &> ||
            std::is_invocable_v<Fn, std::decay_t<T> &, type> ||
            std::is_invocable_v<Fn, std::decay_t<T> &, type &> ||
            std::is_invocable_v<Fn, std::decay_t<T> &, type const> ||
            std::is_invocable_v<Fn, std::decay_t<T> &, type const &> ||
            std::is_invocable_v<Fn, std::decay_t<T> const, type> ||
            std::is_invocable_v<Fn, std::decay_t<T> const, type &> ||
            std::is_invocable_v<Fn, std::decay_t<T> const, type const> ||
            std::is_invocable_v<Fn, std::decay_t<T> const, type const &> ||
            std::is_invocable_v<Fn, std::decay_t<T> const &, type> ||
            std::is_invocable_v<Fn, std::decay_t<T> const &, type &> ||
            std::is_invocable_v<Fn, std::decay_t<T> const &, type const> ||
            std::is_invocable_v<Fn, std::decay_t<T> const &, type const &>;

        return decltype(is_generic_binary < L, Fn, I + 1, prev && cond > ()){};
    }
}

template <typename L, typename Fn>
inline static constexpr bool is_generic_binary_v =
    decltype(is_generic_binary<L, Fn, 0>())::value;

template <typename Pos0Type, typename L, typename Fn>
inline static constexpr bool is_generic_binary_at_pos_1_v =
    decltype(is_generic_binary<Pos0Type, L, Fn, 0>())::value;

} // namespace amt::core

namespace amt::core {

template <typename... Ts>
inline static constexpr bool const
    is_packed_v = boost::mp11::mp_find<boost::mp11::mp_list<Ts...>,
                                       packed_storage_t>::value !=
                  sizeof...(Ts);

template <typename... Ts>
using normalized_list_t =
    boost::mp11::mp_remove<boost::mp11::mp_list<Ts...>, packed_storage_t>;

template <typename L, typename T>
using find_type_t = boost::mp11::mp_find<L, T>;

template <typename L, std::size_t N> using at_c = boost::mp11::mp_at_c<L, N>;

} // namespace amt::core

#endif // AMT_CORE_TYPE_TRAITS_HPP
