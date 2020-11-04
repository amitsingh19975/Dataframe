#if !defined(AMT_DATAFRAME_TRAITS_BOX_TRAITS_HPP)
#define AMT_DATAFRAME_TRAITS_BOX_TRAITS_HPP

#include <type_traits>
#include <variant>

namespace amt {

template <typename... Ts> struct basic_box;

template <typename... Ts> struct overloaded;

} // namespace amt

namespace amt {

template <typename T> struct is_box : std::false_type {};

template <typename... Ts> struct is_box<basic_box<Ts...>> : std::true_type {};

template <typename T>
inline static constexpr bool is_box_v = is_box<std::decay_t<T>>::value;

template <typename T> concept Box = is_box_v<T>;

template<typename T>
struct varaint_from_tuple;

template<typename T>
using varaint_from_tuple_t = typename varaint_from_tuple<T>::type;

template<typename... Ts>
struct varaint_from_tuple< std::tuple<Ts...> >{
    using type = std::variant<Ts...>;
};

template <Box T>
inline static constexpr std::size_t box_size_v =
    std::variant_size_v<typename std::decay_t<T>::base_type>;

template <std::size_t I, Box T>
using box_alternative_t =
    std::variant_alternative_t<I, typename std::decay_t<T>::base_type>;

template <typename T, Box B> struct type_index {
  private:
    static constexpr std::size_t size = box_size_v<B>;

    template <std::size_t I = 0u> static constexpr auto helper() noexcept {
        if constexpr (I < size) {
            using v_type = box_alternative_t<I, B>;
            if constexpr (std::is_same_v<T, v_type>) {
                return std::integral_constant<std::size_t, I>{};
            } else {
                return helper<I + 1u>();
            }
        } else {
            return std::integral_constant<std::size_t,
                                          static_cast<std::size_t>(-1)>{};
        }
    }

  public:
    static constexpr std::size_t value = helper();
};

template <typename T, Box B>
inline static constexpr std::size_t type_index_v =
    type_index<std::decay_t<T>, B>::value;

template <typename T, Box B>
inline static constexpr bool has_type_v = (type_index_v<T, B> !=
                                           static_cast<std::size_t>(-1));

template <typename T> struct is_overloaded : std::false_type {};

template <typename... Ts>
struct is_overloaded<overloaded<Ts...>> : std::true_type {};

template <typename T>
inline static constexpr bool is_overloaded_v =
    is_overloaded<std::decay_t<T>>::value;

template <typename T, typename U>
requires(Box<T> ||
         Box<U>) using box_result_t = std::conditional_t<is_box_v<T>, T, U>;

template<typename T>
inline static constexpr bool is_monostate_v = std::is_same_v< std::decay_t<T>, std::monostate >;

template <typename... Ts>
struct dtype;

template <typename T> struct is_dtype : std::false_type {};

template <typename... Ts> struct is_dtype<dtype<Ts...>> : std::true_type {};

template <typename T>
inline static constexpr bool is_dtype_v = is_dtype<std::decay_t<T>>::value;

template <typename T> concept DType = is_dtype_v<T>;

template <typename U, DType D>
struct dtype_result;

template <typename U, DType D>
using dtype_result_t = typename dtype_result<U,D>::type;

template <typename U, typename T>
struct dtype_result<U, dtype<T>>{
    using type = T;
};

template <typename U>
struct dtype_result<U, dtype<>>{
    using type = U;
};

} // namespace amt

#endif // AMT_DATAFRAME_TRAITS_BOX_TRAITS_HPP
