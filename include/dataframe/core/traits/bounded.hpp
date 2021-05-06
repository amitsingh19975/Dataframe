#if !defined( AMT_DATAFRAME_CORE_TRIATS_BOUNDED_HPP )
#define AMT_DATAFRAME_CORE_TRIATS_BOUNDED_HPP

#include <dataframe/core/traits/storage.hpp>

namespace amt {

    template< typename S, typename... Ts >
    class basic_bounded_storage;

    namespace traits {

        namespace impl {

            template< typename T >
            struct is_bounded_type_storage : std::false_type {};

            template< typename StorageTraits, typename... Ts >
            struct is_bounded_type_storage<
                basic_bounded_storage< StorageTraits, Ts... > >
                : std::true_type {};

        } // namespace impl

        template< typename T >
        concept BoundedTypeStorage = impl::is_bounded_type_storage<
            std::decay_t< std::remove_pointer_t< T > > >::value;

    } // namespace traits

} // namespace amt

#endif // AMT_DATAFRAME_CORE_TRIATS_BOUNDED_HPP
