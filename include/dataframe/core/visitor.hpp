#if !defined( AMT_DATAFRAME_CORE_VISITOR_HPP )
#define AMT_DATAFRAME_CORE_VISITOR_HPP

#include <dataframe/core/traits/bounded.hpp>
#include <dataframe/core/traits/unbounded.hpp>
#include <dataframe/core/traits/variant.hpp>

namespace amt {

    template< typename... Ts >
    struct visitor_list {
        constexpr static auto size = sizeof...( Ts );
    };

    namespace impl {
        template< typename U, traits::UnboundedTypeStorage S, typename FnType >
        constexpr auto visitor_impl( S&& s, FnType&& fn ) noexcept -> void {
            auto&& data = s.template data< U >();
            if ( std::is_invocable_v< FnType, decltype( data.value() ) > ) {
                if ( data )
                    std::invoke( std::forward< FnType >( fn ), data.value() );
            }
        }

    } // namespace impl

    namespace traits {

        template< typename L, typename R >
        struct concat;

        template< typename L, typename R >
        using concat_t = typename concat< L, R >::type;

        template< typename... Ls, typename... Rs >
        struct concat< visitor_list< Ls... >, visitor_list< Rs... > > {
            using type = visitor_list< Ls..., Rs... >;
        };

    } // namespace traits

    template< traits::BoundedTypeStorage S, typename FnType >
    constexpr auto visit( S&& storage, FnType&& fn ) noexcept -> void {
        std::visit( std::forward< FnType >( fn ), storage.base() );
    }

    template< traits::UnboundedTypeStorage S, typename FnType, typename... Ts >
    requires( sizeof...( Ts ) > 0ul ) constexpr auto visit(
        S&& storage, FnType&& fn, visitor_list< Ts... > ) noexcept -> void {
        ( impl::visitor_impl< Ts >( std::forward< S >( storage ), fn ), ... );
    }

    template< typename... Ts, traits::UnboundedTypeStorage S, typename FnType >
    requires( sizeof...( Ts ) >
              0ul ) constexpr auto visit( S&& storage, FnType&& fn ) noexcept
        -> void {
        visit( std::forward< S >( storage ), std::forward< FnType >( fn ),
               visitor_list< Ts... > {} );
    }

} // namespace amt

#endif // AMT_DATAFRAME_CORE_VISITOR_HPP
