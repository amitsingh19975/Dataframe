#if !defined( AMT_DATAFRAME_CORE_TRAITS_VARAINT_LIST_HPP )
#define AMT_DATAFRAME_CORE_TRAITS_VARAINT_LIST_HPP

#include <type_traits>

namespace amt {

    template< typename... Ts >
    struct visitor_list {
        constexpr static auto size = sizeof...( Ts );
    };

    template< typename... Ts >
    struct visitor_base {
        using visitor_list_t = visitor_list< Ts... >;
    };

    namespace traits {

        template< typename L, typename R >
        struct concat;

        template< typename L, typename R >
        using concat_t = typename concat< L, R >::type;

        template< typename... Ls, typename... Rs >
        struct concat< visitor_list< Ls... >, visitor_list< Rs... > > {
            using type = visitor_list< Ls..., Rs... >;
        };

        namespace impl {

            template< typename T >
            struct is_visitor_list : std::false_type {};

            template< typename T >
            constexpr static bool is_visitor_list_v =
                is_visitor_list< T >::value;

            template< typename... Ts >
            struct is_visitor_list< visitor_list< Ts... > > : std::true_type{};

            template<typename T, typename... Ts>
            constexpr auto in_visitor_list_helper(visitor_list<Ts...>) noexcept -> bool{
                return (std::is_same_v<T,Ts> || ...);
            }

        } // namespace impl

        template< typename T >
        concept VisitorBase = requires {
            typename T::visitor_list;
            requires impl::is_visitor_list_v<typename T::visitor_list>;
        };

        template<typename T, typename List>
        concept InVisitorList = impl::in_visitor_list_helper<T>(List{});

    } // namespace traits

} // namespace amt

#endif // AMT_DATAFRAME_CORE_TRAITS_VARAINT_LIST_HPP
