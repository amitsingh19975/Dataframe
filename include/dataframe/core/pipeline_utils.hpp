#if !defined(AMT_DATAFRAME_TRAITS_PIPELINE_UTILS_HPP)
#define AMT_DATAFRAME_TRAITS_PIPELINE_UTILS_HPP

#include <dataframe/traits/pipeline_traits.hpp>

namespace amt {

template<typename Fn, typename In>
struct invoke_fn_result : std::invoke_result<Fn,In>{};

template<typename Fn, typename... Ts>
struct invoke_fn_result< Fn, std::tuple<Ts...> > : std::invoke_result< Fn, Ts... >{};

template<typename Fn, typename In>
using invoke_fn_result_t = typename invoke_fn_result<std::decay_t<Fn>, In>::type;

template <typename Fn, typename In>
constexpr decltype(auto) invoke_fn(Fn &&fn, In &&in) noexcept {
    if constexpr (is_pipeline_v<Fn>) {
        return std::invoke(std::forward<Fn>(fn), std::forward<In>(in));
    } else {
        return std::apply(std::forward<Fn>(fn), std::forward<In>(in));
    }
}

template <std::size_t I = 0u, typename TupleType, typename Input>
requires(is_tuple_v<Input> &&is_tuple_v<TupleType>) constexpr decltype(auto)
    invoke_pipeline_left_to_right(TupleType &&t, Input &&in) noexcept {
    constexpr auto sz = std::tuple_size_v<std::decay_t<TupleType>>;
    if constexpr (sz > I + 1u) {
        decltype(auto) fn = std::get<I>(t);
        decltype(auto) next_in =
            invoke_fn(std::forward<decltype(fn)>(fn), std::forward<Input>(in));
        return invoke_pipeline_left_to_right<I + 1>(
            std::forward<TupleType>(t), std::forward_as_tuple(std::forward<decltype(next_in)>(next_in)));

    } else if constexpr (sz == I + 1u) {
        decltype(auto) fn = std::get<I>(t);
        return invoke_fn(std::forward<decltype(fn)>(fn),
                         std::forward<Input>(in));
    } else {
        return in;
    }
}

template <std::size_t I = 0u, typename TupleType, typename Input>
requires(is_tuple_v<Input> &&is_tuple_v<TupleType>) constexpr decltype(auto)
    invoke_pipeline_right_to_left(TupleType &&t, Input &&in) noexcept {
    constexpr auto sz = std::tuple_size_v<std::decay_t<TupleType>>;
    if constexpr (sz > I + 1u) {
        auto &fn = std::get<sz - I - 1u>(t);
        decltype(auto) next_in =
            invoke_fn(std::forward<decltype(fn)>(fn), std::forward<Input>(in));
        return invoke_pipeline_right_to_left<I + 1>(
            std::forward<TupleType>(t), std::forward_as_tuple(std::forward<decltype(next_in)>(next_in)));
    } else if constexpr (sz == I + 1u) {
        auto &fn = std::get<sz - I - 1u>(t);
        return invoke_fn(std::forward<decltype(fn)>(fn),
                         std::forward<Input>(in));
    } else {
        return in;
    }
}

template <typename TupleType, typename Input>
requires(is_tuple_v<TupleType> &&is_tuple_v<
         Input>) constexpr decltype(auto) invoke_pipeline(TupleType &&t,
                                                Input &&in) noexcept {
    return invoke_pipeline_left_to_right(std::forward<TupleType>(t),
                                         std::forward<Input>(in));
}

template <typename TupleType, typename Input>
requires(is_tuple_v<TupleType> &&is_tuple_v<
         Input>) constexpr decltype(auto) invoke_pipeline(TupleType &&t, Input &&in,
                                                tag::right_to_left_t) noexcept {
    return invoke_pipeline_right_to_left(std::forward<TupleType>(t),
                                         std::forward<Input>(in));
}

template<typename... Ts>
constexpr decltype(auto) make_args(Ts&&... args){
    return std::forward_as_tuple(std::forward<Ts>(args)...);
}

} // namespace amt

#endif // AMT_DATAFRAME_TRAITS_PIPELINE_UTILS_HPP
