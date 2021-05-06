#if !defined( AMT_DATAFRAME_CORE_TRIATS_OPERATOR_HPP )
#define AMT_DATAFRAME_CORE_TRIATS_OPERATOR_HPP

#include <concepts>

namespace amt { namespace traits {

    template< typename T >
    concept Addable = requires( T t ) {
        { t + t } -> std::same_as< T >;
    };

    template< typename T >
    concept Subtractable = requires( T t ) {
        { t - t } -> std::same_as< T >;
    };

    template< typename T >
    concept Multipliable = requires( T t ) {
        { t * t } -> std::same_as< T >;
    };

    template< typename T >
    concept Divisible = requires( T t ) {
        { t / t } -> std::same_as< T >;
    };

    template< typename T, typename U >
    concept EqualityComparable =
        requires( std::remove_reference_t< T > const& t,
                  std::remove_reference_t< U > const& u ) {
        { t == u } -> std::same_as< bool >;
        { u == t } -> std::same_as< bool >;
        { t != u } -> std::same_as< bool >;
        { u != t } -> std::same_as< bool >;
    };

    template< typename T, typename... Args >
    concept CallableObject = requires( std::remove_reference_t< T > const& t,
                                       Args&&... args ) {
        { t( std::forward< Args >( args )... ) };
    };

}} // namespace amt::traits

#endif // AMT_DATAFRAME_CORE_TRIATS_OPERATOR_HPP
