#if !defined( AMT_DATAFRAME_CORE_TRIATS_SERIES_HPP )
#define AMT_DATAFRAME_CORE_TRIATS_SERIES_HPP

#include <dataframe/core/traits/bounded.hpp>
#include <dataframe/core/traits/unbounded.hpp>

namespace amt {
    template< typename S >
    class basic_series;

    namespace traits {

        namespace impl {

            template< typename T >
            struct is_series : std::false_type {};

            template< typename StorageType >
            struct is_series< basic_series< StorageType > > : std::true_type {};

        } // namespace impl

        template< typename T >
        concept Series = impl::is_series<
            std::decay_t< std::remove_pointer_t< T > > >::value;

    } // namespace traits

} // namespace amt

#endif // AMT_DATAFRAME_CORE_TRIATS_SERIES_HPP
