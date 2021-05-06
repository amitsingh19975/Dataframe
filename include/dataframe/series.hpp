#if !defined(AMT_DATAFRAME_SERIES_HPP)
#define AMT_DATAFRAME_SERIES_HPP

#include <dataframe/core/traits/bounded.hpp>
#include <dataframe/core/traits/traits.hpp>
#include <initializer_list>
namespace amt{

    template<typename StorageType>
    class basic_series{
        static_assert(traits::Storage<StorageType>, "amt::basic_series<StorageType> : StorageType must satisfy Storage Trait");

    public:
        constexpr static bool is_static_storage = StorageType::is_static_storage;
        using base_type = StorageType;
        using size_type = typename base_type::size_type;
        using difference_type = typename base_type::difference_type;

        constexpr basic_series() noexcept = default;
        constexpr basic_series(basic_series const&) = default;
        constexpr basic_series(basic_series &&) noexcept = default;
        constexpr basic_series& operator=(basic_series const&) = default;
        constexpr basic_series& operator=(basic_series &&) noexcept = default;
        constexpr ~basic_series() noexcept = default;

        constexpr basic_series(base_type const& data) noexcept
            : m_data(data)
        {}

        constexpr basic_series(base_type && data) noexcept
            : m_data(std::move(data))
        {}

        template<typename T>
        constexpr basic_series(std::initializer_list<T> li) noexcept
            : m_data(std::move(li))
        {}

        template<typename T>
            requires (!is_static_storage)
        constexpr basic_series(size_type size, T def) noexcept
            : m_data(size, std::move(def))
        {}

        constexpr auto& base() noexcept{ return m_data; }
        constexpr auto const& base() const noexcept{ return m_data; }

        constexpr auto operator==(basic_series const& other) const noexcept -> bool{
            return m_data == other.m_data;
        }

        constexpr auto operator!=(basic_series const& other) const noexcept -> bool{
            return !(*this == other);
        }

    private:
        base_type m_data{};
    };


} // namespace amt


#endif // AMT_DATAFRAME_SERIES_HPP
