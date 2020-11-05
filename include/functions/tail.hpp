#if !defined(AMT_DATAFRAME_FUNCTIONS_TAIL_HPP)
#define AMT_DATAFRAME_FUNCTIONS_TAIL_HPP

#include <core/macro.hpp>
#include <traits/series_traits.hpp>
#include <slice.hpp>

namespace amt {

namespace fn{
    
    struct tail_t{
        constexpr auto operator()(Series auto&& s, std::size_t size = 5u) const{
            auto sz = std::min(s.size(), size);
            return s( amt::slice(s.size() - sz) );
        }
        
        constexpr auto operator()(Frame auto&& f, std::size_t size = 5u) const{
            auto sz = std::min(f.rows(), size);
            return f( amt::slice{}, amt::slice(f.rows() - sz) );
        }
    };

} // namespace fn

inline static constexpr auto tail = fn::tail_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_TAIL_HPP
