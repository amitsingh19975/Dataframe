#if !defined( AMT_DATAFRAME_CORE_TRIATS_VARAINT_HPP )
#define AMT_DATAFRAME_CORE_TRIATS_VARAINT_HPP

#include <dataframe/core/storage.hpp>
#include <dataframe/core/traits/bounded.hpp>
#include <variant>

namespace amt { namespace traits {

    namespace impl {

        template< typename T, typename V, std::size_t... Is >
        constexpr auto
        is_type_in_variant_helper( std::index_sequence< Is... > ) noexcept
            -> bool {
            return ( std::is_same_v< T, std::variant_alternative_t< Is, V > > ||
                     ... );
        }

    } // namespace impl

    template< typename T, typename V >
    concept IsTypeInVariant = impl::is_type_in_variant_helper< T, V >(
        std::make_index_sequence< std::variant_size_v< V > > {} );
}} // namespace amt::traits

#endif // AMT_DATAFRAME_CORE_TRIATS_VARAINT_HPP
