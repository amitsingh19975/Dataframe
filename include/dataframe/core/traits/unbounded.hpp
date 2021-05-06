#if !defined( AMT_DATAFRAME_CORE_TRIATS_UNBOUNDED_HPP )
#define AMT_DATAFRAME_CORE_TRIATS_UNBOUNDED_HPP

#include <dataframe/core/traits/storage.hpp>

namespace amt {

    template< typename S, std::ptrdiff_t TypesUpperBound >
    class basic_unbounded_storage;

    namespace traits {

        namespace impl {

            template< typename T >
            struct is_unbounded_storage : std::false_type {};

            template< typename StorageTraits, std::ptrdiff_t TypesUpperBound >
            struct is_unbounded_storage<
                basic_unbounded_storage< StorageTraits, TypesUpperBound > >
                : std::true_type {};

        } // namespace impl

        template< typename T >
        concept UnboundedTypeStorage = impl::is_unbounded_storage<
            std::decay_t< std::remove_pointer_t< T > > >::value;

    } // namespace traits

} // namespace amt

#endif // AMT_DATAFRAME_CORE_TRIATS_UNBOUNDED_HPP
