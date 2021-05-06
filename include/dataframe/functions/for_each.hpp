#if !defined( AMT_DATAFRAME_FUNCTIONS_FOR_EACH_HPP )
#define AMT_DATAFRAME_FUNCTIONS_FOR_EACH_HPP

#include <dataframe/core/visitor.hpp>
#include <dataframe/functions/function_base.hpp>

namespace amt::fn {

    template< typename FnType, typename TypeList >
    struct for_each : impl::fn_base< for_each< FnType, TypeList > > {
        using base_type = impl::fn_base< for_each< FnType, TypeList > >;

        constexpr for_each( FnType&& fn ) noexcept
            : m_fn( std::forward< FnType >( fn ) ) {}

        constexpr for_each( FnType&& fn, TypeList ) noexcept
            : m_fn( std::forward< FnType >( fn ) ) {}

        template< typename S >
        requires( traits::BoundedTypeStorage< S > ||
                  (traits::UnboundedTypeStorage< S > &&
                   !std::is_same_v< TypeList, tag::dummy >)) constexpr auto
        operator()( S&& s ) const noexcept -> void {
            visit(
                std::forward< S >( s ),
                [ this ]< typename T >( T&& c ) {
                    for ( auto&& el : c ) {
                        using el_type = decltype( el );
                        base_type::invoke_if_invocable(
                            m_fn, std::forward< el_type >( el ) );
                    }
                },
                TypeList {} );
        }

        template< traits::Series S >
        constexpr auto operator()( S&& s ) const noexcept -> void {
            this->operator()( s.base() );
        }

    private:
        FnType const& m_fn;
    };

} // namespace amt::fn

namespace amt {

    template< typename... Ts, typename FnType >
    constexpr auto for_each( FnType&& fn ) noexcept {
        if constexpr ( sizeof...( Ts ) > 0ul )
            return fn::for_each( std::forward< FnType >( fn ),
                                 visitor_list< Ts... > {} );
        else
            return fn::for_each( std::forward< FnType >( fn ), tag::dummy {} );
    }

    template< typename FnType, typename... Ts >
    constexpr auto for_each( FnType&& fn, visitor_list< Ts... > li ) noexcept {
        return fn::for_each( std::forward< FnType >( fn ), li );
    }

    template< typename... Ts, typename FnType >
    constexpr auto for_each( auto&& s, FnType&& fn ) noexcept {
        for_each<Ts...>(std::forward<FnType>(fn))(s);
    }

    template< typename FnType, typename... Ts >
    constexpr auto for_each( auto&& s, FnType&& fn, visitor_list< Ts... > li ) noexcept {
        for_each( std::forward< FnType >( fn ), li )(s);
    }

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_FOR_EACH_HPP