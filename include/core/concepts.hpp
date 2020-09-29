#if !defined(AMT_CORE_CONCEPTS_HPP)
#define AMT_CORE_CONCEPTS_HPP

#include <core/type_traits.hpp>

namespace amt::core {

template <typename T, typename U>
concept HasEq = has_comp_equal_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasLess = has_comp_less_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasGt = has_comp_greater_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasLessEq = has_comp_less_equal_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasGtEq = has_comp_greater_equal_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasPlus = has_op_plus_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasMinus = has_op_minus_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasMul = has_op_mul_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasDiv = has_op_div_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasPlusEq = has_op_plus_equal_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasMinusEq = has_op_minus_equal_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasMulEq = has_op_mul_equal_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T, typename U>
concept HasDivEq = has_op_div_equal_v<std::decay_t<T>, std::decay_t<U>>;

template <typename T>
concept HasStdToString = has_std_to_string_v<std::decay_t<T>>;

template <typename T> concept Storage = is_storage_v<std::decay_t<T>>;

} // namespace amt::core

namespace amt {

template <typename T> concept Box = is_box_v<std::decay_t<T>>;

template <typename T> concept Series = is_series_v<std::decay_t<T>>;

template <typename T> concept Frame = is_frame_v<std::decay_t<T>>;

template <typename T> concept View = is_view_v<std::decay_t<T>>;

template <typename T> concept Slice = is_slice_v<std::decay_t<T>>;

template <typename T> concept Tag = is_tag_v<std::decay_t<T>>;

template <typename T> concept SeriesView = is_series_view_v<std::decay_t<T>>;

template <typename T> concept FrameView = is_frame_view_v<std::decay_t<T>>;

template <typename T> concept SeriesViewOrSeries = Series<T> || SeriesView<T>;

template <typename T> concept FrameViewOrFrame = Frame<T> || FrameView<T>;

} // namespace amt

#endif // AMT_CORE_CONCEPTS_HPP
