#if !defined( AMT_DATAFRAME_CORE_BOUNDED_STORAGE_HELPER_HPP )
#define AMT_DATAFRAME_CORE_BOUNDED_STORAGE_HELPER_HPP

#include <dataframe/core/traits/bounded.hpp>
#include <dataframe/core/traits/variant_list.hpp>
#include <typeindex>

namespace amt {

    namespace impl {

        template< traits::VisitorList List, std::size_t I = 0ul >
        auto type_index_from_variant_index( std::size_t id ) noexcept
            -> std::type_index {
            auto def         = std::type_index( typeid( void ) );
            constexpr auto N = List::size;
            switch ( id ) {
                case I + 0: {
                    if constexpr ( I + 0 < N ) {
                        using type = variant_list_element_t< I + 0, List >;
                        return { typeid( type ) };
                    }
                }

                case I + 1: {
                    if constexpr ( I + 1 < N ) {
                        using type = variant_list_element_t< I + 1, List >;
                        return { typeid( type ) };
                    }
                }

                case I + 2: {
                    if constexpr ( I + 2 < N ) {
                        using type = variant_list_element_t< I + 2, List >;
                        return { typeid( type ) };
                    }
                }
                case I + 3: {
                    if constexpr ( I + 3 < N ) {
                        using type = variant_list_element_t< I + 3, List >;
                        return { typeid( type ) };
                    }
                }
                case I + 4: {
                    if constexpr ( I + 4 < N ) {
                        using type = variant_list_element_t< I + 4, List >;
                        return { typeid( type ) };
                    }
                }
                case I + 5: {
                    if constexpr ( I + 5 < N ) {
                        using type = variant_list_element_t< I + 5, List >;
                        return { typeid( type ) };
                    }
                }
                case I + 6: {
                    if constexpr ( I + 6 < N ) {
                        using type = variant_list_element_t< I + 6, List >;
                        return { typeid( type ) };
                    }
                }
                case I + 7: {
                    if constexpr ( I + 7 < N ) {
                        using type = variant_list_element_t< I + 7, List >;
                        return { typeid( type ) };
                    }
                }
                case I + 8: {
                    if constexpr ( I + 8 < N ) {
                        using type = variant_list_element_t< I + 8, List >;
                        return { typeid( type ) };
                    }
                }
                case I + 9: {
                    if constexpr ( I + 9 < N ) {
                        using type = variant_list_element_t< I + 9, List >;
                        return { typeid( type ) };
                    }
                }
                default: {
                    if constexpr ( I + 10 < N ) {
                        return type_index_from_variant_index< List, I + 10 >(
                            id );
                    }
                }
            }

            return def;
        }

    } // namespace impl

} // namespace amt

#endif // AMT_DATAFRAME_CORE_BOUNDED_STORAGE_HELPER_HPP
