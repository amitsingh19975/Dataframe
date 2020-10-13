#if !defined(AMT_FUNCTIONS_HPP)
#define AMT_FUNCTIONS_HPP

#include <functions/accumulate.hpp>
#include <functions/drop_cols.hpp>
#include <functions/drop_rows.hpp>
#include <functions/filter.hpp>
#include <functions/to.hpp>
#include <functions/head.hpp>
#include <functions/tail.hpp>
#include <functions/transform.hpp>

namespace amt {

using index_list = std::unordered_set<std::size_t>;
using name_list = std::unordered_set<std::string_view>;

inline static constexpr auto const transform = amt::fn::transform_fn{};
inline static constexpr auto const accumulate = amt::fn::accumulate_fn{};
inline static constexpr auto const filter = amt::fn::filter_fn{};
inline static constexpr auto const drop_rows = amt::fn::drop_rows_fn{};
inline static constexpr auto const drop_cols = amt::fn::drop_cols_fn{};
inline static constexpr auto const head = amt::fn::head_fn{};
inline static constexpr auto const tail = amt::fn::tail_fn{};
template <typename To>
inline static constexpr auto const to = amt::fn::to_fn<To>{};

} // namespace amt

namespace amt{
    
    namespace detail{
        template<typename T = double, typename Fn>
        constexpr T sum(SeriesViewOrSeries auto& in, Fn&& fn) noexcept{
            if( in.empty() ) return {};
            else if( !(in.is_integer() || in.is_floating_point()) ) return {};
            auto sum = accumulate(in, T{}, [fn = std::move(fn)]<typename U>(auto const& res, U const& val){
                if constexpr( std::is_convertible_v<U,T> ){
                    auto v = static_cast<T>(val);
                    auto r = static_cast<T>(res);
                    return fn(r,v);
                }else{
                    return res;
                }
            });
            return sum;
        }
    } // namespace detail
    
    
    template<typename T = double>
    constexpr T mean(SeriesViewOrSeries auto& in) noexcept{
        auto s = detail::sum(in,std::plus<T>{});
        auto N = static_cast<T>(in.size());
        return s / N;
    }
    
    template<typename T = double>
    constexpr T var(SeriesViewOrSeries auto& in) noexcept{
        auto s = detail::sum(in,[](auto const& res, auto const& val){
            return res + val * val;
        });
        auto N = static_cast<T>(in.size());
        auto m = mean(in);
        m *= m;
        return s / N - m;
    }
    
    // sample mean
    template<typename T = double>
    constexpr T smean(SeriesViewOrSeries auto& in) noexcept{
        auto s = detail::sum(in,std::plus<T>{});
        auto N = static_cast<T>(in.size());
        return s / (N - 1);
    }
    
    // sample variance
    template<typename T = double>
    constexpr T svar(SeriesViewOrSeries auto& in) noexcept{
        auto s = detail::sum(in,[](auto const& res, auto const& val){
            return res + val * val;
        });
        auto N = static_cast<T>(in.size());
        auto m = smean(in);
        m *= m;
        return s / (N - 1) - m;
    }

} // namespace amt


#endif // AMT_FUNCTIONS_HPP
