#if !defined( AMT_DATAFRAME_CORE_BOUNDED_STORAGE_HPP )
#define AMT_DATAFRAME_CORE_BOUNDED_STORAGE_HPP

#include <dataframe/core/iterator_wrapper.hpp>
#include <dataframe/core/traits/bounded.hpp>
#include <dataframe/core/traits/operator.hpp>
#include <dataframe/core/traits/variant.hpp>
#include <dataframe/core/visitor.hpp>
#include <initializer_list>
#include <string>

namespace amt {

    template< typename S, typename... Ts >
    class basic_bounded_storage {
        static_assert(
            traits::StorageTrait< S >,
            "The first template argument, S must statisfy Storage Traits" );
        using storage_trait_type = S;

    public:
        using size_type       = typename storage_trait_type::size_type;
        using difference_type = typename storage_trait_type::difference_type;
        template< typename T >
        using storage_type = typename storage_trait_type::template rebind< T >;

        using type_list = visitor_list< storage_type< Ts >... >;
        using base_type = std::variant< storage_type< Ts >... >;

        constexpr static size_type number_of_types = sizeof...( Ts );
        constexpr static bool is_static_storage =
            traits::StaticStorageTrait< storage_trait_type >;

        constexpr basic_bounded_storage() noexcept = default;
        constexpr basic_bounded_storage( basic_bounded_storage const& ) =
            default;
        constexpr basic_bounded_storage( basic_bounded_storage&& ) noexcept =
            default;
        constexpr basic_bounded_storage&
        operator=( basic_bounded_storage const& ) = default;
        constexpr basic_bounded_storage&
        operator=( basic_bounded_storage&& ) noexcept = default;
        constexpr ~basic_bounded_storage() noexcept   = default;

        template< typename T >
        requires traits::IsTypeInVariant< storage_type< T >, base_type >
        constexpr basic_bounded_storage( storage_type< T > const& vec )
            : m_data( vec ) {}

        template< typename T >
        requires traits::IsTypeInVariant< storage_type< T >, base_type >
        constexpr basic_bounded_storage( storage_type< T >&& vec )
            : m_data( std::move( vec ) ) {}

        template< typename T >
        requires(
            traits::IsTypeInVariant< storage_type< T >, base_type > &&
            !is_static_storage ) constexpr basic_bounded_storage( std::
                                                                      initializer_list<
                                                                          T >
                                                                          li )
            : m_data( storage_type< T >( std::move( li ) ) ) {}

        template< typename T >
        requires(
            traits::IsTypeInVariant< storage_type< T >, base_type >&&
                is_static_storage ) constexpr basic_bounded_storage( std::
                                                                         initializer_list<
                                                                             T >
                                                                             li ) {
            if ( li.size() >= storage_trait_type::size ) {
                throw std::runtime_error(
                    "amt::bounded_storage(std::initializer_list<T>) : "
                    "the size of the initializer list is greator than "
                    "the static storage" );
            }
            auto& temp = m_data.template emplace< storage_type< T > >();
            std::copy( li.begin(), li.end(), temp.begin() );
        }

        template< typename T >
        requires(
            traits::IsTypeInVariant< storage_type< T >, base_type > &&
            !is_static_storage ) constexpr basic_bounded_storage( size_type
                                                                      size,
                                                                  T def )
            : m_data( storage_type< T >( size, std::move( def ) ) ) {}

        constexpr auto& base() noexcept { return m_data; }
        constexpr auto const& base() const noexcept { return m_data; }

        friend auto swap( basic_bounded_storage& lhs,
                          basic_bounded_storage& rhs ) noexcept -> void {
            std::swap( lhs.m_data, rhs.m_data );
        }

        constexpr auto size() const noexcept -> size_type {
            return visit( *this, []( auto&& cont ) { return cont.size(); } );
        }

        constexpr auto
        operator==( basic_bounded_storage const& other ) const noexcept
            -> bool {
            if ( !holds_same_type( *this, other ) )
                return false;
            auto res = false;
            return binary_op(
                *this, other,
                [ &res ]< typename T, typename U >( T&& l, U&& r ) {
                    using ltype = typename std::decay_t< T >::value_type;
                    using rtype = typename std::decay_t< U >::value_type;
                    if constexpr ( traits::EqualityComparable< ltype,
                                                               rtype > ) {
                        res = std::equal( l.begin(), l.end(), r.begin() );
                    }
                } );
            return res;
        }

        constexpr auto
        operator!=( basic_bounded_storage const& other ) const noexcept
            -> bool {
            return !( *this == other );
        }

        constexpr size_type index() const noexcept { return m_data.index(); }

    private:
        base_type m_data {};
    };

    template< traits::StorageTrait S, typename... Ts >
    using bounded_storage =
        basic_bounded_storage< S, char, short int, int, long int, long long int,
                               unsigned char, unsigned short int, unsigned int,
                               unsigned long int, unsigned long long int, float,
                               double, std::string, Ts... >;

    using dynamic_bounded_storage = bounded_storage< dynamic_storage<> >;

    template< std::size_t N >
    using static_bounded_storage = bounded_storage< static_storage< N > >;

} // namespace amt

#endif // AMT_DATAFRAME_CORE_BOUNDED_STORAGE_HPP
