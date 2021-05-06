#if !defined( AMT_DATAFRAME_FUNCTIONS_PUSH_BACK_HPP )
#define AMT_DATAFRAME_FUNCTIONS_PUSH_BACK_HPP

#include <dataframe/functions/function_base.hpp>
#include <dataframe/core/common_functions.hpp>

namespace amt::fn {

    struct push_back : impl::fn_base< push_back > {
        using base_type = impl::fn_base< push_back >;

        template<typename T, traits::BoundedTypeStorage S>
        requires (!std::decay_t<S>::is_static_storage)
        constexpr auto operator()(S&& s, T&& val) const noexcept{
            ::amt::get<T>(s).push_back(std::forward<T>(val));
        }
        
        template<typename T, traits::UnboundedTypeStorage S>
        requires (!std::decay_t<S>::is_static_storage)
        constexpr auto operator()(S&& s, T&& val) const noexcept{
            s.template get_or_make<T>().value().push_back(std::forward<T>(val));
        }
        
        template<typename T, traits::Series S>
        constexpr auto operator()(S&& s, T&& val) const noexcept{
            this->operator()(s.base(), std::forward<T>(val));
        }
    };

} // namespace amt::fn

namespace amt {

    constexpr static auto push_back = fn::push_back{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_PUSH_BACK_HPP