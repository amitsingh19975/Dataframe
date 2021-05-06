#if !defined( AMT_DATAFRAME_SERIES_HPP )
#define AMT_DATAFRAME_SERIES_HPP

#include <dataframe/core/traits/bounded.hpp>
#include <dataframe/core/traits/traits.hpp>
#include <initializer_list>
namespace amt {

    template< typename StorageType >
    class basic_series {
        static_assert(
            traits::Storage< StorageType >,
            "amt::basic_series<StorageType> : StorageType must satisfy "
            "Storage Trait" );

    public:
        constexpr static bool is_static_storage =
            StorageType::is_static_storage;
        using base_type          = StorageType;
        using size_type          = typename base_type::size_type;
        using difference_type    = typename base_type::difference_type;
        using name_type          = std::string;
        using optional_name_type = std::optional< std::string >;

        constexpr basic_series() noexcept                 = default;
        constexpr basic_series( basic_series const& )     = default;
        constexpr basic_series( basic_series&& ) noexcept = default;
        constexpr basic_series& operator=( basic_series const& ) = default;
        constexpr basic_series& operator=( basic_series&& ) noexcept = default;
        constexpr ~basic_series() noexcept                           = default;

        template< typename... Ts >
        requires std::is_constructible_v< base_type, Ts... >
        constexpr basic_series( Ts&&... args ) noexcept
            : m_data( std::forward< Ts >( args )... ) {}

        template< typename... Ts >
        requires std::is_constructible_v< base_type, Ts... >
        constexpr basic_series( std::string_view name, Ts&&... args ) noexcept
            : m_data( std::forward< Ts >( args )... ), m_name( name ) {}

        template< typename T >
        constexpr basic_series( std::initializer_list< T > li ) noexcept
            : m_data( std::move( li ) ) {}

        constexpr auto& base() noexcept { return m_data; }
        constexpr auto const& base() const noexcept { return m_data; }

        constexpr auto operator==( basic_series const& other ) const noexcept
            -> bool {
            return m_data == other.m_data;
        }

        constexpr auto operator!=( basic_series const& other ) const noexcept
            -> bool {
            return !( *this == other );
        }

    private:
        base_type m_data {};
        optional_name_type m_name {};
    };

} // namespace amt

#endif // AMT_DATAFRAME_SERIES_HPP
