#if !defined(AMT_DATAFRAME_CORE_TRAITS_STORAGE_HPP)
#define AMT_DATAFRAME_CORE_TRAITS_STORAGE_HPP

#include <type_traits>
#include <dataframe/core/tags.hpp>
#include <concepts>


namespace amt::traits{
    
    template<typename T>
    concept StorageTrait = requires {
        typename T::storage_tag;
        typename T::storage_type;
        typename T::value_type;
        typename T::size_type;
        typename T::difference_type;
        typename T::template rebind<int>;
    };
    
    template<typename T>
    concept StaticStorageTrait = requires {
        requires StorageTrait<T>;
        requires std::is_same_v<typename T::storage_tag, tag::static_storage>;
        {T::size};
    };
    
    template<typename T>
    concept DynamicStorageTrait = requires {
        requires StorageTrait<T>;
        requires std::is_same_v<typename T::storage_tag, tag::dynamic_storage>;
    };

} // namespace amt::traits



#endif // AMT_DATAFRAME_CORE_TRAITS_STORAGE_HPP
