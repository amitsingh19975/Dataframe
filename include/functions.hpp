#if !defined(AMT_FUNCTIONS_HPP)
#define AMT_FUNCTIONS_HPP

#include <functions/accumulate.hpp>
#include <functions/drop_cols.hpp>
#include <functions/drop_rows.hpp>
#include <functions/filter.hpp>
#include <functions/to.hpp>
#include <functions/transform.hpp>

namespace amt {

using index_list = std::unordered_set<std::size_t>;
using name_list = std::unordered_set<std::string_view>;

inline static constexpr auto const transform = amt::fn::transform_fn{};
inline static constexpr auto const accumulate = amt::fn::accumulate_fn{};
inline static constexpr auto const filter = amt::fn::filter_fn{};
inline static constexpr auto const drop_rows = amt::fn::drop_rows_fn{};
inline static constexpr auto const drop_cols = amt::fn::drop_cols_fn{};
template <typename To>
inline static constexpr auto const to = amt::fn::to_fn<To>{};

} // namespace amt

#endif // AMT_FUNCTIONS_HPP
