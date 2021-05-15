#if !defined( AMT_DATAFRAME_FUNCTIONS_REDUCE_HPP )
#define AMT_DATAFRAME_FUNCTIONS_REDUCE_HPP

#include <dataframe/functions/for_each.hpp>

namespace amt::fn {

    template<typename Acc, typename TypeList, typename FnType>
    struct reduce : impl::fn_base< reduce<Acc,TypeList,FnType> > {
        using base_type = impl::fn_base< reduce<Acc,TypeList,FnType> >;

        constexpr reduce(Acc acc, FnType fn, TypeList) noexcept
            : m_fn(std::move(fn))
            , m_acc(std::move(acc))
        {}

        template<traits::DataFrame SIn>
        constexpr auto operator()(SIn const& in) const -> Acc{
            auto res = m_acc;
            bool has_excuted = false;
            for_each([&res,&fn = m_fn, &has_excuted]<typename U>(U&& acc){
                using type = std::decay_t<U>;
                if constexpr( std::is_invocable_v<FnType,Acc,type>){
                    res = std::invoke(fn, res, acc);
                    has_excuted = true;
                }
            }, TypeList{})(in);

            if(!has_excuted){
                throw std::runtime_error(
                    "amt::fn::reduce : "
                    "unable to reduce the range, please check the function parameters or types"
                );
            }
            return res;
        }

    private:
        FnType m_fn;
        Acc m_acc;
    };

} // namespace amt::fn

namespace amt {

    template<typename... Ts, typename Acc, typename FnType>
    constexpr auto reduce(Acc&& acc, FnType&& fn) noexcept{
        if constexpr(sizeof...(Ts) > 0)
            return fn::reduce(std::forward<Acc>(acc), std::forward<FnType>(fn), visitor_list<Ts...>{});
        else
            return fn::reduce(std::forward<Acc>(acc), std::forward<FnType>(fn), visitor_list<std::decay_t<Acc>>{});
    }

    template<typename... Ts, typename Acc, typename FnType>
    constexpr auto reduce(Acc&& acc, FnType&& fn, visitor_list< Ts... > li) noexcept{
        return fn::reduce(std::forward<Acc>(acc), std::forward<FnType>(fn), li);
    }

    template<typename... Ts, typename Acc, typename FnType>
    constexpr auto reduce(auto&& s, Acc&& acc, FnType&& fn) noexcept{
        if constexpr(sizeof...(Ts) > 0)
            return fn::reduce(std::forward<Acc>(acc), std::forward<FnType>(fn), visitor_list<Ts...>{})(s);
        else
            return fn::reduce(std::forward<Acc>(acc), std::forward<FnType>(fn), visitor_list<std::decay_t<Acc>>{})(s);
    }

    template<typename... Ts, typename Acc, typename FnType>
    constexpr auto reduce(auto&& s, Acc&& acc, FnType&& fn, visitor_list< Ts... > li) noexcept{
        return fn::reduce(std::forward<Acc>(acc), std::forward<FnType>(fn), li)(s);
    }


} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_REDUCE_HPP