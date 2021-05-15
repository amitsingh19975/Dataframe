#if !defined( AMT_DATAFRAME_FUNCTIONS_TRANSFORM_HPP )
#define AMT_DATAFRAME_FUNCTIONS_TRANSFORM_HPP

#include <dataframe/functions/for_each.hpp>

namespace amt::fn {

    template<typename TypeList, typename FnType>
    struct transform : impl::fn_base< transform<TypeList,FnType> > {
        using base_type = impl::fn_base< transform<TypeList,FnType> >;

        constexpr transform(FnType fn, TypeList) noexcept
            : m_fn(std::move(fn))
        {}

        template<traits::DataFrame SIn, traits::DataFrame SOut>
        constexpr auto operator()(SIn const& in, SOut& out) const -> void{
            if(in.size() != out.size()){
                throw std::length_error("amt::fn::transform : in and out should have same length");
            }

            bool has_excuted = false;

            binary_op(in.base(), out.base(), [&fn = m_fn, &has_excuted]<typename LHS, typename RHS>(LHS const& l, RHS& r){
                using rtype = typename RHS::value_type;
                using ltype = typename LHS::value_type;
                constexpr auto is_invocable = std::is_invocable_v<FnType,ltype>;
                if constexpr(std::is_same_v<rtype,ltype> && is_invocable){
                    std::transform(l.begin(), l.end(), r.begin(), fn);
                    has_excuted = true;
                }
            }, TypeList{});

            if(!has_excuted){
                throw std::runtime_error(
                    "amt::fn::transform : "
                    "unable to transform the range, please check the function parameters or types"
                );
            }
        }

        template<traits::DataFrame SInOut>
        constexpr auto operator()(SInOut& in) const -> void{
            this->operator()(in,in);
        }

    private:
        FnType m_fn;
    };

} // namespace amt::fn

namespace amt {

    template<typename... Ts, typename FnType>
    constexpr auto transform(FnType&& fn) noexcept{
        if constexpr(sizeof...(Ts) > 0)
            return fn::transform(std::forward<FnType>(fn), visitor_list<Ts...>{});
        else
            return fn::transform(std::forward<FnType>(fn), tag::dummy{});
    }

    template<typename TypeList, typename FnType>
    constexpr auto transform(FnType&& fn, TypeList) noexcept{
        return fn::transform(std::forward<FnType>(fn), TypeList{});
    }

    template<typename... Ts, typename SInOut, typename FnType>
    constexpr auto transform(SInOut& in, FnType&& fn) noexcept{
        return transform<Ts...>(std::forward<FnType>(fn))(in);
    }

    template<typename TypeList, typename SInOut, typename FnType>
    constexpr auto transform(SInOut& in, FnType&& fn) noexcept{
        return transform(std::forward<FnType>(fn), TypeList{})(in);
    }

    template<typename... Ts, typename SIn, typename SOut, typename FnType>
    constexpr auto transform(SIn const& in, SOut& out, FnType&& fn) noexcept{
        return transform<Ts...>(std::forward<FnType>(fn))(in, out);
    }

    template<typename TypeList, typename SIn, typename SOut, typename FnType>
    constexpr auto transform(SIn const& in, SOut& out, FnType&& fn, TypeList) noexcept{
        return transform(std::forward<FnType>(fn), TypeList{})(in, out);
    }

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_TRANSFORM_HPP