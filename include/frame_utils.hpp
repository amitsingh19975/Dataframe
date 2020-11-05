#if !defined(AMT_DATAFRAME_FRAME_UTILS_HPP)
#define AMT_DATAFRAME_FRAME_UTILS_HPP

#include <core/utils.hpp>
#include <box_utils.hpp>
#include <traits/frame_traits.hpp>

namespace amt {

namespace arg {

struct col {
    constexpr col() noexcept = default;
    constexpr col(col const &other) noexcept = default;
    constexpr col(col &&other) noexcept = default;
    constexpr col &operator=(col const &other) noexcept = default;
    constexpr col &operator=(col &&other) noexcept = default;
    constexpr ~col() noexcept = default;

    constexpr col(std::size_t i) noexcept : m_idx(i) {}

    constexpr std::size_t operator()() const noexcept { return m_idx; }

  private:
    std::size_t m_idx{};
};

struct row {
    constexpr row() noexcept = default;
    constexpr row(row const &other) noexcept = default;
    constexpr row(row &&other) noexcept = default;
    constexpr row &operator=(row const &other) noexcept = default;
    constexpr row &operator=(row &&other) noexcept = default;
    constexpr ~row() noexcept = default;

    constexpr row(std::size_t i) noexcept : m_idx(i) {}

    constexpr std::size_t operator()() const noexcept { return m_idx; }

  private:
    std::size_t m_idx{};
};

} // namespace arg


} // namespace amt

#endif // AMT_DATAFRAME_FRAME_UTILS_HPP
