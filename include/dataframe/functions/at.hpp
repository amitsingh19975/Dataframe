#if !defined( AMT_DATAFRAME_FUNCTIONS_AT_HPP )
#define AMT_DATAFRAME_FUNCTIONS_AT_HPP

#include <dataframe/functions/function_base.hpp>
#include <dataframe/core/common_functions.hpp>

namespace amt::fn {

    template<typename T>
    struct at : impl::fn_base< at<T> > {
        using base_type = impl::fn_base< at<T> >;
        using size_type = std::size_t;

        template<traits::BoundedTypeStorage S>
        constexpr decltype(auto) operator()(S&& s, size_type pos) const{
            return ::amt::get<T>(s).at(pos);
        }
        
        template<traits::UnboundedTypeStorage S>
        constexpr decltype(auto) operator()(S&& s, size_type pos) const{
            return s.template get_or_make<T>().value().at(pos);
        }
        
        template<traits::Series S>
        constexpr decltype(auto) operator()(S&& s, size_type pos) const{
            return this->operator()(s.base(), pos);
        }
    };

} // namespace amt::fn

namespace amt {

    template<typename T>
    constexpr static auto at = fn::at<T>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_AT_HPP