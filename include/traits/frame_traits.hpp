#if !defined(AMT_DATAFRAME_TRAITS_FRAME_TRAITS_HPP)
#define AMT_DATAFRAME_TRAITS_FRAME_TRAITS_HPP

namespace amt
{

template<Box V>
struct basic_frame;

template <Box V> struct basic_frame_view;

} // namespace amt

namespace amt
{

template<typename T>
struct is_frame : std::false_type{};

template<Box V>
struct is_frame< basic_frame<V> > : std::true_type{};

template<Box V>
struct is_frame< basic_frame_view<V> > : std::true_type{};

template<typename T>
inline static constexpr bool is_frame_v = is_frame< std::decay_t<T> >::value;

template<typename T>
concept Frame = is_frame_v<T>;

template <typename T> struct is_frame_view : std::false_type {};

template <Box V>
struct is_frame_view<basic_frame_view<V>> : std::true_type {};

template <typename T>
inline static constexpr bool is_frame_view_v =
    is_frame_view<std::decay_t<T>>::value;

template<typename T>
concept FrameView = is_frame_view_v<T>;

template<typename T>
inline static constexpr bool is_pure_frame_v = is_frame_v< T> && !is_frame_view_v<T>;

template<typename T>
concept PureFrame = is_pure_frame_v<T>;

template<typename T>
struct frame_type;

template<PureFrame T>
struct frame_type<T>{
    using type = T;
};

template<FrameView T>
struct frame_type<T>{
    using type = typename T::view_of;
};

template<typename T>
using frame_type_t = typename frame_type<T>::type;

template <typename... Ts>
struct frame_result;

template <Frame T, typename U>
requires(!Frame<U>)
struct frame_result<T,U>{
    using type = frame_type_t<T>;
};

template <typename T, Frame U>
requires(!Frame<T>)
struct frame_result<T,U>{
    using type = frame_type_t<U>;
};

template <Frame T, Frame U>
struct frame_result<T,U>{
    using type = frame_type_t<T>;
};

template <Frame T>
struct frame_result<T>{
    using type = frame_type_t<T>;
};

template <typename... Ts> using frame_result_t = typename frame_result< std::decay_t<Ts>... >::type;

template <Frame L, Frame R>
inline static constexpr bool is_frame_same_v =
    std::is_same_v<std::decay_t<typename L::box_type>,
                   std::decay_t<typename R::box_type>>;


} // namespace amt


#endif // AMT_DATAFRAME_TRAITS_FRAME_TRAITS_HPP
