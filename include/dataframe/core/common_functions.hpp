#if !defined(AMT_DATAFRAME_CORE_COMMON_FUNCTIONS_HPP)
#define AMT_DATAFRAME_CORE_COMMON_FUNCTIONS_HPP

#include <dataframe/core/traits/traits.hpp>
#include <dataframe/core/iterator_wrapper.hpp>
#include <dataframe/core/visitor.hpp>
    
    
namespace amt{

    // enum Cmp{
    //     NotEqual,
    //     Equal,
    //     Less,
    //     Greater
    // };
    
    template<typename T, traits::BoundedTypeStorage S>
    constexpr decltype(auto) get(S&& storage){
        using decayed_storage_t = std::decay_t<S>;
        using variant_type = typename decayed_storage_t::base_type;
        using type = typename decayed_storage_t::template storage_type<T>;
        static_assert(traits::IsTypeInVariant<type,variant_type>, "amt::get(S&&) : type is not inside the variant set");
        return std::get<type>(storage.base());
    }

    template<typename T, traits::UnboundedTypeStorage S>
    constexpr decltype(auto) get(S&& storage){
        return storage.template get_or_make<T>().value();
    }

    template<typename T, traits::Series S>
    constexpr decltype(auto) get(S&& storage){
        return ::amt::get<T>(storage.base());
    }

    template<typename T, typename S>
    constexpr decltype(auto) begin(S&& storage){
        return ::amt::get<T>(::std::forward<S>(storage)).begin();
    }
    
    template<typename T, typename S>
    constexpr decltype(auto) end(S&& storage){
        return ::amt::get<T>(::std::forward<S>(storage)).end();
    }

    template<typename T, typename S>
    constexpr auto iter(S&& storage){
       return iterator_wrapper{amt::begin<T>(::std::forward<S>(storage)),::amt::end<T>(std::forward<S>(storage))};
    }

    template<typename T>
    constexpr auto equal(auto&& LHS, auto&& RHS) noexcept -> bool{
        auto const& lhs = ::amt::get<T>(LHS);
        auto const& rhs = ::amt::get<T>(RHS);
        if(lhs.size() != rhs.size()) return false;

        if( std::equal(lhs.begin(), lhs.end(), rhs.begin()) ) return true;
        else return false;
    }

    template<typename T, traits::BoundedTypeStorage S>
    constexpr auto is(S&& s) noexcept -> bool{
        return std::holds_alternative<T>(s.base());
    }

    constexpr auto holds_same_type(auto&& l, auto&& r) noexcept -> bool{
        return l.base().index() == r.base().index();
    }

    template<typename FnType>
    constexpr auto binary_op(traits::BoundedTypeStorage auto&& LHS, traits::BoundedTypeStorage auto&& RHS, FnType&& fn) noexcept -> void{
        visit(LHS, [&RHS, fn = std::forward<FnType>(fn)](auto&& l){
            visit(RHS, [&fn, &l](auto&& r){
                if constexpr(std::is_invocable_v<FnType,decltype(l), decltype(r)>){
                    std::invoke(fn, l, r);
                }
            });
        });
    }

} // namespace amt


#endif // AMT_DATAFRAME_CORE_COMMON_FUNCTIONS_HPP
