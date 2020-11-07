#if !defined(AMT_DATAFRAME_TRAITS_PIPELINE_TRAITS_HPP)
#define AMT_DATAFRAME_TRAITS_PIPELINE_TRAITS_HPP

#include <dataframe/core/tag.hpp>
#include <type_traits>

namespace amt {

template <typename... Fs> struct pipeline;

template <typename T> struct is_pipeline : std::false_type {};

template <typename... Fs>
struct is_pipeline<pipeline<Fs...>> : std::true_type {};

template <typename T>
inline static constexpr bool is_pipeline_v =
    is_pipeline<std::decay_t<T>>::value;

template <typename T> concept Pipeline = is_pipeline_v<T>;

template <Pipeline... Ps> struct connect_pipelines;

template <typename T> struct is_connect_pipelines : std::false_type {};

template <typename... Fs>
struct is_connect_pipelines<pipeline<Fs...>> : std::true_type {};

template <typename T>
inline static constexpr bool is_connect_pipelines_v =
    is_connect_pipelines<std::decay_t<T>>::value;

template <typename T>
inline static constexpr bool is_right_to_left_v =
    std::is_same_v<std::decay_t<T>, tag::right_to_left_t>;

} // namespace amt

#endif // AMT_DATAFRAME_TRAITS_PIPELINE_TRAITS_HPP
