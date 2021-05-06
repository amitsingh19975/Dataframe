#if !defined(AMT_DATAFRAME_CORE_TRIATS_TRAITS_HPP)
#define AMT_DATAFRAME_CORE_TRIATS_TRAITS_HPP

#include <dataframe/core/traits/variant.hpp>
#include <dataframe/core/traits/operator.hpp>
#include <dataframe/core/traits/view.hpp>

namespace amt{

    namespace traits{
        
        template<typename T>
        concept Storage = BoundedTypeStorage<T> || UnboundedTypeStorage<T>;

        template<typename T>
        concept DataFrame = Series<T> || Frame<T>;

    } // namespace traits

} // namespace amt



#endif // AMT_DATAFRAME_CORE_TRIATS_TRAITS_HPP
