#if !defined(AMT_DATAFRAME_FUNCTIONS_REDUCE_HPP)
#define AMT_DATAFRAME_FUNCTIONS_REDUCE_HPP

#include <box_utils.hpp>
#include <core/macro.hpp>
#include <traits/series_traits.hpp>

namespace amt {

namespace fn{
    
    struct reduce_t{
        template <typename Accu, typename Fn>
        constexpr Accu operator()(Series auto const &in, Accu acc, Fn &&fn) const{

        #pragma omp parallel for schedule(static, 1)
            for (auto it = in.begin(); it != in.end(); ++it) {
                acc = std::invoke(fn, acc, get<Accu>(*it));
            }

            return acc;
        }
    };

} // namespace fn

inline static constexpr auto reduce = fn::reduce_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_REDUCE_HPP
