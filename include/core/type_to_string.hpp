#if !defined(AMT_CORE_TYPE_TO_STRING_HPP)
#define AMT_CORE_TYPE_TO_STRING_HPP

#include <boost/core/demangle.hpp>
#include <cstddef>
#include <utility>

namespace amt::core {


template <typename T>[[nodiscard]] inline std::string type_to_string(T &&val) {
    using type = std::decay_t<T>;
#define AMT_BEGIN_DEFAULT_TYPES(MESS)                                          \
    if constexpr (false)                                                       \
        return "NONE";
#define AMT_END_DEFAULT_TYPES(MESS)                                            \
    else return boost::core::demangle(typeid(val).name());
#define AMT_DEFINE_DEFAULT_TYPES(TYPE, MESS, NAME)                             \
    else if constexpr (std::is_same_v<type, TYPE>) return MESS;
#include <core/default_types.def>
}

template <typename T>[[nodiscard]] inline std::string type_to_string() {
    return type_to_string(T{});
}

} // namespace amt::core

#endif // AMT_CORE_TYPE_TO_STRING_HPP
