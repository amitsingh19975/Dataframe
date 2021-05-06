#if !defined(AMT_DATAFRAME_CORE_TRIATS_FRAME_HPP)
#define AMT_DATAFRAME_CORE_TRIATS_FRAME_HPP

#include <dataframe/core/traits/unbounded.hpp>
#include <dataframe/core/traits/bounded.hpp>

namespace amt{

    template<typename S>
    class basic_frame;

    namespace traits{
        
        namespace impl{

            template<typename T>
            struct is_frame : std::false_type{};
            
            template<typename StorageType>
            struct is_frame< basic_frame<StorageType> > : std::true_type{};

        } // namespace impl

        template<typename T>
        concept Frame = impl::is_frame<std::decay_t<std::remove_pointer_t<T>>>::value;
        
    } // namespace traits
    
} // namespace amt

#endif // AMT_DATAFRAME_CORE_TRIATS_FRAME_HPP
