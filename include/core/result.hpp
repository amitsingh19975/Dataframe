#if !defined(AMT_CORE_RESULT_HPP)
#define AMT_CORE_RESULT_HPP

#include <core/error.hpp>
#include <optional>

namespace amt {

template <typename R> struct result {
    using result_type = R;
    using error_type = error_code;
    constexpr result() = default;
    constexpr result(result const &other) = default;
    constexpr result(result &&other) = default;
    constexpr result &operator=(result const &other) = default;
    constexpr result &operator=(result &&other) = default;
    ~result() = default;
    constexpr result(error_type e) noexcept
        : m_data(std::nullopt), m_err(std::move(e)) {}

    template <typename U>
    requires std::is_same_v<std::decay_t<U>, R> constexpr result(U &&val)
        : m_data(std::forward<U>(val)), m_err(error_type::NONE) {}

    constexpr auto &operator*() { return *m_data; }

    constexpr auto const &operator*() const { return *m_data; }

    constexpr bool has_value() const noexcept { return m_data.has_value(); }

    constexpr result_type value_or(result_type &&r) const noexcept {
        return m_data.value_or(std::forward<result_type>(r));
    }

    constexpr bool has_err() const noexcept {
        return !m_data.has_value() || (m_err != error_type::NONE);
    }

    constexpr char const *what() const noexcept { return to_string(m_err); }

    constexpr error_type const &err() const noexcept { return m_err; }

    constexpr operator bool() const noexcept { return !has_err(); }

    constexpr void reset() {
        m_data = std::nullopt;
        m_err = error_code::NONE;
    }

    constexpr bool operator==(result const &rhs) const noexcept {
        return has_err() && rhs.has_err() && (*m_data == *rhs);
    }

    constexpr bool operator!=(result const &rhs) const noexcept {
        return !(*this == rhs);
    }

  private:
    std::optional<result_type> m_data{};
    error_type m_err{};
};

} // namespace amt

#endif // AMT_CORE_RESULT_HPP
