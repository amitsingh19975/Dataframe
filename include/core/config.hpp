#if !defined(AMT_CORE_CONFIG_HPP)
#define AMT_CORE_CONFIG_HPP

namespace amt::core {

enum class os { NONE, WINDOWS, MACOS, LINUX };

struct config {
#if !defined(NDEBUG)
    static constexpr auto debug = true;
#else
    static constexpr auto debug = false;
#endif

#if defined(_Win32) || defined(_Win64)
    static constexpr auto os = os::WINDOWS;
     
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
        static constexpr auto os = os::LINUX;
#endif

#if defined(__APPLE__) || defined(__MACH__)
    static constexpr auto os = os::MACOS;
#endif

#if !defined(_Win32) && !defined(_Win64) && !defined(linux) &&                 \
    !defined(__linux) && !defined(__linux__) && !defined(__APPLE__) &&         \
    !defined(__MACH__)
    static constexpr auto os = os::None;
#endif
};

} // namespace amt::core

#endif // AMT_CORE_CONFIG_HPP
