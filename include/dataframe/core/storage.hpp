#if !defined( AMT_DATAFRAME_CORE_STORAGE_HPP )
#define AMT_DATAFRAME_CORE_STORAGE_HPP

#include "tags.hpp"

#include <array>
#include <vector>

namespace amt {

    template< typename T >
    struct storage_traits;

} // namespace amt

namespace amt {

    namespace impl {

        template< typename T, typename... Ts >
        struct rebind_storage;

        template< typename T, typename A, typename... Ts >
        struct rebind_storage< std::vector< T, A >, Ts... > {
            using type = std::vector< Ts... >;
        };

        template< typename T, std::size_t N, typename U >
        struct rebind_storage< std::array< T, N >, U > {
            using type = std::array< U, N >;
        };

        template< typename T, typename... Ts >
        using rebind_storage_t = typename rebind_storage< T, Ts... >::type;

    } // namespace impl

    template< typename T, typename A >
    struct storage_traits< std::vector< T, A > > {
        using storage_tag     = tag::dynamic_storage;
        using storage_type    = std::vector< T, A >;
        using allocator_type  = A;
        using value_type      = T;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;

    private:
        template< typename U >
        using rebind_alloc = typename std::allocator_traits<
            allocator_type >::template rebind_alloc< U >;

    public:
        template< typename U >
        using rebind =
            impl::rebind_storage_t< storage_type, U, rebind_alloc< U > >;
    };

    template< typename T, std::size_t N >
    struct storage_traits< std::array< T, N > > {
        using storage_tag     = tag::static_storage;
        using storage_type    = std::array< T, N >;
        using value_type      = T;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;

        constexpr static auto size = N;

        template< typename U >
        using rebind = impl::rebind_storage_t< storage_type, U >;
    };

    template< typename A = std::allocator< tag::dummy > >
    using dynamic_storage =
        storage_traits< std::vector< typename A::value_type, A > >;

    template< std::size_t N >
    using static_storage = storage_traits< std::array< amt::tag::dummy, N > >;

} // namespace amt

#endif // AMT_DATAFRAME_CORE_STORAGE_HPP
