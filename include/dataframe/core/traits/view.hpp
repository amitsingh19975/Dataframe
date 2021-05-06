#if !defined(AMT_DATAFRAME_CORE_TRIATS_VIEW_HPP)
#define AMT_DATAFRAME_CORE_TRIATS_VIEW_HPP

#include <dataframe/core/traits/frame.hpp>
#include <dataframe/core/traits/series.hpp>

namespace amt
{
    template<typename T>
    class basic_view;

    namespace traits{
        
        namespace impl{

            template<typename T>
            struct is_view : std::false_type{};
            
            template<typename StorageType>
            struct is_view< basic_view<StorageType> > : std::true_type{};

        } // namespace impl

        template<typename T>
        concept View = impl::is_view<std::decay_t<std::remove_pointer_t<T>>>::value;
        
    } // namespace traits
    
} // namespace amt


#endif // AMT_DATAFRAME_CORE_TRIATS_VIEW_HPP
