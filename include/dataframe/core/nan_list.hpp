#if !defined(AMT_DATAFRAME_CORE_NAN_LIST_HPP)
#define AMT_DATAFRAME_CORE_NAN_LIST_HPP

#include <array>
#include <string>
#include <tuple>

namespace amt {
inline static constexpr std::array<std::string_view, 18u> default_nan_list = {
    "",     "#N/A", "#N/A N/A", "#NA",     "-1.#IND", "-1.#QNAN",
    "-NaN", "-nan", "1.#IND",   "1.#QNAN", "<NA>",    "N/A",
    "NA",   "NULL", "NaN",      "n/a",     "nan",     "null"};

constexpr std::pair<bool, std::size_t>
find_in_nan_list(std::string_view name) noexcept {
    auto i = 0ul;
    for (auto const &el : default_nan_list) {
        if (el == name)
            return {true, i};
        ++i;
    }
    return {false, i};
}

} // namespace amt

#endif // AMT_DATAFRAME_CORE_NAN_LIST_HPP
