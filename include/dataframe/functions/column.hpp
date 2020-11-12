#if !defined(AMT_DATAFRAME_FUNCTIONS_COLUMN_HPP)
#define AMT_DATAFRAME_FUNCTIONS_COLUMN_HPP

#include <dataframe/core/utils.hpp>
#include <string>

namespace amt {

namespace fn {

struct column_t {

    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

    constexpr column_t(std::string_view name) noexcept : m_col_name(name) {}

    constexpr column_t(std::size_t idx) noexcept : m_idx(idx) {}

    constexpr column_t(column_t const &other) noexcept = default;
    constexpr column_t(column_t &&other) noexcept = default;
    constexpr column_t &operator=(column_t const &other) noexcept = default;
    constexpr column_t &operator=(column_t &&other) noexcept = default;
    constexpr ~column_t() noexcept = default;

    constexpr bool has_index() const noexcept { return m_idx != npos; }

    constexpr bool index() const noexcept { return m_idx; }

    constexpr std::string_view name() const noexcept { return m_col_name; }

  private:
    std::string_view m_col_name{};
    std::size_t m_idx{npos};
};

} // namespace fn

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_COLUMN_HPP
