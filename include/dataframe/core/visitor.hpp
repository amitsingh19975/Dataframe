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

    } // namespace impl

    template< traits::BoundedTypeStorage S, typename FnType >
    constexpr auto visit( S&& storage, FnType&& fn ) noexcept -> void {
        std::visit( std::forward< FnType >( fn ), storage.base() );
    }

    template< traits::BoundedTypeStorage S, typename FnType,  typename... Ts >
        requires (sizeof... (Ts) > 0ul)
    constexpr auto visit( S&& storage, FnType&& fn, visitor_list< Ts... > ) noexcept -> void {
        using visitor_list_t = visitor_list< Ts... >;
        using variant_type = typename std::decay_t<S>::base_type;
        
        static_assert( (traits::IsTypeInVariant<Ts,variant_type> && ...), "type is not in the set of bounded type storage" );

        std::visit([fn = std::forward<FnType>(fn)]<typename T>(T&& val){
            if constexpr(traits::InVisitorList<std::decay_t<T>, visitor_list_t>){
                std::invoke(fn, std::forward<T>(val));
            }
        }, storage.base());
    }

    template< typename... Ts, traits::BoundedTypeStorage S, typename FnType >
        requires (sizeof... (Ts) > 0ul)
    constexpr auto visit( S&& storage, FnType&& fn ) noexcept -> void {
        visit(std::forward<S>(storage), std::forward<FnType>(fn), visitor_list<Ts...>{});
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
