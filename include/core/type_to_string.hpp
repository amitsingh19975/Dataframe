#if !defined(AMT_CORE_TYPE_TO_STRING_HPP)
#define AMT_CORE_TYPE_TO_STRING_HPP

#include <boost/core/demangle.hpp>
#include <cstddef>

namespace amt::core {

template <typename T>[[nodiscard]] inline std::string type_to_string() {
    using type = std::decay_t<T>;
    if constexpr (std::is_same_v<type, std::int8_t>) {
        return std::string("std::int8_t");
    } else if constexpr (std::is_same_v<type, std::int16_t>) {
        return std::string("std::int16_t");
    } else if constexpr (std::is_same_v<type, std::int32_t>) {
        return std::string("std::int32_t");
    } else if constexpr (std::is_same_v<type, std::int64_t>) {
        return std::string("std::int64_t");
    } else if constexpr (std::is_same_v<type, std::uint8_t>) {
        return std::string("std::uint8_t");
    } else if constexpr (std::is_same_v<type, std::uint16_t>) {
        return std::string("std::uint16_t");
    } else if constexpr (std::is_same_v<type, std::uint32_t>) {
        return std::string("std::uint32_t");
    } else if constexpr (std::is_same_v<type, std::uint64_t>) {
        return std::string("std::uint64_t");
    } else if constexpr (std::is_same_v<type, std::size_t>) {
        return std::string("std::size_t");
    } else if constexpr (std::is_same_v<type, std::ptrdiff_t>) {
        return std::string("std::ptrdiff_t");
    } else if constexpr (std::is_same_v<type, float>) {
        return std::string("float");
    } else if constexpr (std::is_same_v<type, double>) {
        return std::string("double");
    } else if constexpr (std::is_same_v<type, std::string>) {
        return std::string("std::string");
    } else {
        return boost::core::demangle(typeid(std::declval<T>()).name());
    }
}

template <typename T>[[nodiscard]] inline std::string type_to_string(T &&val) {
    using type = std::decay_t<T>;
    if constexpr (std::is_same_v<type, std::int8_t>) {
        return std::string("std::int8_t");
    } else if constexpr (std::is_same_v<type, std::int16_t>) {
        return std::string("std::int16_t");
    } else if constexpr (std::is_same_v<type, std::int32_t>) {
        return std::string("std::int32_t");
    } else if constexpr (std::is_same_v<type, std::int64_t>) {
        return std::string("std::int64_t");
    } else if constexpr (std::is_same_v<type, std::uint8_t>) {
        return std::string("std::uint8_t");
    } else if constexpr (std::is_same_v<type, std::uint16_t>) {
        return std::string("std::uint16_t");
    } else if constexpr (std::is_same_v<type, std::uint32_t>) {
        return std::string("std::uint32_t");
    } else if constexpr (std::is_same_v<type, std::uint64_t>) {
        return std::string("std::uint64_t");
    } else if constexpr (std::is_same_v<type, std::size_t>) {
        return std::string("std::size_t");
    } else if constexpr (std::is_same_v<type, std::ptrdiff_t>) {
        return std::string("std::ptrdiff_t");
    } else if constexpr (std::is_same_v<type, float>) {
        return std::string("float");
    } else if constexpr (std::is_same_v<type, double>) {
        return std::string("double");
    } else if constexpr (std::is_same_v<type, std::string>) {
        return std::string("std::string");
    } else {
        return boost::core::demangle(typeid(val).name());
    }
}

} // namespace amt::core

#endif // AMT_CORE_TYPE_TO_STRING_HPP
