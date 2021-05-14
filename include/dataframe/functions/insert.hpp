#if !defined( AMT_DATAFRAME_FUNCTIONS_INSERT_HPP )
#define AMT_DATAFRAME_FUNCTIONS_INSERT_HPP

#include <dataframe/core/common_functions.hpp>
#include <dataframe/functions/function_base.hpp>
#include <initializer_list>

namespace amt::fn {

    struct insert : impl::fn_base< insert > {
        using base_type = impl::fn_base< insert >;
        using size_type = std::size_t;

        template< typename T, traits::BoundedTypeStorage S >
        requires( !std::decay_t< S >::is_static_storage ) constexpr auto
        operator()( S&& s, size_type pos, T&& val ) const {
            auto& cont = ::amt::get< T >( s );
            this->helper( cont, pos, std::forward< T >( val ) );
        }

        template< typename T, traits::UnboundedTypeStorage S >
        requires( !std::decay_t< S >::is_static_storage ) constexpr auto
        operator()( S&& s, size_type pos, T&& val ) const {
            auto& cont = s.template get_or_make< T >().value();
            this->helper( cont, pos, std::forward< T >( val ) );
        }

        template< typename T, traits::BoundedTypeStorage S >
        requires( !std::decay_t< S >::is_static_storage ) constexpr auto
        operator()( S&& s, size_type pos,
                    std::initializer_list< T > li ) const {
            auto& cont = ::amt::get< T >( s );
            this->helper( cont, pos, li.begin(), li.end() );
        }

        template< typename T, traits::UnboundedTypeStorage S >
        requires( !std::decay_t< S >::is_static_storage ) constexpr auto
        operator()( S&& s, size_type pos,
                    std::initializer_list< T > li ) const {
            auto& cont = s.template get_or_make< T >().value();
            this->helper( cont, pos, li.begin(), li.end() );
        }

        template< typename T, traits::Series S >
        constexpr auto operator()( S&& s, size_type pos,
                                   T&& val ) const {
            this->operator()( s.base(), pos, std::forward< T >( val ) );
        }

        template< typename T, traits::Series S >
        constexpr auto
        operator()( S&& s, size_type pos,
                    std::initializer_list< T > li ) const {
            this->operator()( s.base(), pos, li );
        }

    private:
        template< typename Iterator >
        constexpr auto helper( auto& cont, size_type pos, Iterator first,
                               Iterator last ) const -> void {
            auto iter = cont.begin() + static_cast< std::ptrdiff_t >( pos );
            cont.insert( iter, first, last );
        }

        template< typename T >
        constexpr auto helper( auto& cont, size_type pos,
                               T&& val ) const -> void {
            auto iter = cont.begin() + static_cast< std::ptrdiff_t >( pos );
            cont.insert( iter, std::forward< T >( val ) );
        }
    };

} // namespace amt::fn

namespace amt {

    constexpr static auto insert = fn::insert {};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_INSERT_HPP