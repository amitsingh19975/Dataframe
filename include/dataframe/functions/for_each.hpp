#if !defined( AMT_DATAFRAME_FUNCTIONS_FOR_EACH_HPP )
#define AMT_DATAFRAME_FUNCTIONS_FOR_EACH_HPP

#include <dataframe/core/visitor.hpp>
#include <dataframe/functions/function_base.hpp>

namespace amt::fn {

    template< typename FnType >
    struct for_each : impl::fn_base< for_each< FnType > > {
        using base_type = impl::fn_base< for_each< FnType > >;

        constexpr for_each( FnType&& fn ) noexcept
            : m_fn( std::forward< FnType >( fn ) ) {}

        template< traits::Series S >
        requires traits::VisitorBase< FnType >
        constexpr decltype( auto ) operator()( S&& s ) const noexcept {
            using visitor_list_t =
                typename std::decay_t< FnType >::visitor_list_t;
            this->operator()( std::forward< S >( s ), visitor_list_t {} );
        }

        template< traits::Series S >
        requires traits::BoundedTypeStorage<
            typename std::decay_t< S >::base_type >
        constexpr decltype( auto ) operator()( S&& s ) const noexcept {
            visit( s.base(),
                   [ fn = std::forward< FnType >( m_fn ) ]( auto&& c ) {
                       for ( auto&& el : c ) {
                           using el_type = decltype( el );
                           base_type::invoke_if_invocable(
                               fn, std::forward< el_type >( el ) );
                       }
                   } );
        }

        template< traits::Series S, typename... Ts >
        constexpr decltype( auto )
        operator()( S&& s, visitor_list< Ts... > li ) const noexcept {
            visit(
                s.base(),
                [ fn = std::forward< FnType >( m_fn ) ]( auto&& c ) {
                    for ( auto&& el : c ) {
                        using el_type = decltype( el );
                        base_type::invoke_if_invocable(
                            fn, std::forward< el_type >( el ) );
                    }
                },
                li );
        }

    private:
        FnType&& m_fn;
    };

} // namespace amt::fn

#endif // AMT_DATAFRAME_FUNCTIONS_FOR_EACH_HPP