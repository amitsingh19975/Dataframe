#if !defined(AMT_DATAFRAME_FUNCTIONS_HEAD_HPP)
#define AMT_DATAFRAME_FUNCTIONS_HEAD_HPP

#include <core/macro.hpp>
#include <traits/series_traits.hpp>
#include <slice.hpp>

namespace amt {

namespace fn{
    
    struct head_t{
        constexpr auto operator()(Series auto&& s, std::size_t size = 5u) const{
            auto sz = std::min(s.size(), size);
            return s( amt::slice(0u, sz - 1u, 1u) );
        }
        
        constexpr auto operator()(Frame auto&& f, std::size_t size = 5u) const{
            auto sz = std::min(f.rows(), size);
            return f( amt::slice{}, amt::slice(0u, sz - 1u, 1u) );
        }
    };

} // namespace fn

inline static constexpr auto head = fn::head_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_HEAD_HPP
