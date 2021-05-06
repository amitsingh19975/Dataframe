#if !defined( AMT_DATAFRAME_FUNCTIONS_FUNCTION_BASE_HPP )
#define AMT_DATAFRAME_FUNCTIONS_FUNCTION_BASE_HPP

#include <dataframe/core/traits/traits.hpp>

namespace amt::fn::impl {

    template< typename Child >
    struct fn_base {
        using child_type = Child;

        constexpr auto self() noexcept -> Child& {
            return static_cast< Child& >( *this );
        }

        constexpr auto self() const noexcept -> Child const& {
            return static_cast< Child const& >( *this );
        }

        template< typename... Args >
        requires traits::CallableObject< Child, Args&&... >
        constexpr decltype( auto ) operator()( Args&&... args ) const noexcept {
            return std::invoke( self(), std::forward< Args >( args )... );
        }

        template< typename FnType, typename... Args >
        constexpr static decltype( auto )
        invoke_if_invocable( FnType&& fn, Args&&... args ) noexcept {
            if constexpr ( traits::CallableObject< FnType, Args&&... > )
                return std::invoke( std::forward< FnType >( fn ),
                                    std::forward< Args >( args )... );
        }

        constexpr fn_base() noexcept                 = default;
        constexpr fn_base( fn_base const& ) noexcept = delete;
        constexpr fn_base( fn_base&& ) noexcept      = default;
        constexpr fn_base& operator=( fn_base const& ) noexcept = delete;
        constexpr fn_base& operator=( fn_base&& ) noexcept = default;
        constexpr ~fn_base() noexcept                      = default;
    };

} // namespace amt::fn::impl

#endif // AMT_DATAFRAME_FUNCTIONS_FUNCTION_BASE_HPP
