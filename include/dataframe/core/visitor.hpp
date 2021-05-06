#if !defined( AMT_DATAFRAME_CORE_VISITOR_HPP )
#define AMT_DATAFRAME_CORE_VISITOR_HPP

#include <dataframe/core/traits/bounded.hpp>
#include <dataframe/core/traits/unbounded.hpp>
#include <dataframe/core/traits/variant.hpp>
#include <dataframe/core/traits/variant_list.hpp>

namespace amt {
    namespace impl {
        template< typename U, traits::UnboundedTypeStorage S, typename FnType >
        constexpr auto visitor_impl( S&& s, FnType&& fn ) noexcept -> void {
            auto&& data = s.template data< U >();
            if ( std::is_invocable_v< FnType, decltype( data.value() ) > ) {
                if ( data )
                    std::invoke( std::forward< FnType >( fn ), data.value() );
            }

        }

        template< traits::BoundedTypeStorage S, typename FnType, typename... Ts >
        requires( sizeof...( Ts ) > 0ul ) constexpr auto visitor_impl(
            S&& storage, FnType&& fn, visitor_list< Ts... > ) noexcept -> void {
            using visitor_list_t = visitor_list< Ts... >;

            static_assert(
                ( traits::InVisitorList< Ts,
                                        typename std::decay_t< S >::type_list > &&
                ... ),
                "type is not in the set of bounded type storage" );

            std::visit(
                [ fn = std::forward< FnType >( fn ) ]< typename T >( T&& val ) {
                    using storage_traits_t = storage_traits< std::decay_t< T > >;
                    if constexpr ( traits::InVisitorList<
                                    typename storage_traits_t::value_type,
                                    visitor_list_t > ) {
                        std::invoke( fn, std::forward< T >( val ) );
                    }
                },
                storage.base() );
        }

        template< traits::BoundedTypeStorage S, typename FnType >
        constexpr auto visitor_impl( S&& storage, FnType&& fn, tag::dummy) noexcept -> void {
            std::visit( std::forward< FnType >( fn ), storage.base() );
        }
    } // namespace impl


    template< typename... Ts, traits::BoundedTypeStorage S, typename FnType >
    requires( sizeof...( Ts ) >
              0ul ) constexpr auto visit( S&& storage, FnType&& fn ) noexcept
        -> void {
        impl::visitor_impl( std::forward< S >( storage ), std::forward< FnType >( fn ),
               visitor_list< Ts... > {} );
    }

    template< traits::BoundedTypeStorage S, typename FnType, typename TypeList = tag::dummy >
    constexpr auto visit( S&& storage, FnType&& fn, TypeList = {} ) noexcept
        -> void {
        impl::visitor_impl( std::forward< S >( storage ), std::forward< FnType >( fn ),
            TypeList{} );
    }

    template< traits::UnboundedTypeStorage S, typename FnType, typename... Ts >
    requires( sizeof...( Ts ) > 0ul ) constexpr auto visit(
        S&& storage, FnType&& fn, visitor_list< Ts... > ) noexcept -> void {
        ( impl::visitor_impl< Ts >( std::forward< S >( storage ), fn ), ... );
    }

    template< traits::UnboundedTypeStorage S, typename FnType, typename... Ts >
    requires( sizeof...( Ts ) >
              0ul ) constexpr auto visit( S&& storage, FnType&& fn ) noexcept
        -> void {
        visit( std::forward< S >( storage ), std::forward< FnType >( fn ),
               visitor_list< Ts... > {} );
    }

} // namespace amt

#endif // AMT_DATAFRAME_CORE_VISITOR_HPP
