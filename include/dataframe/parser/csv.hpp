#if !defined(AMT_PARSER_CSV_HPP)
#define AMT_PARSER_CSV_HPP

#include <dataframe/parser/dsv.hpp>

namespace amt{
    
    template<Box B = box>
    inline auto read_csv(std::string_view filename, bool use_header_from_csv = false){
        return read_dsv<B>(filename,use_header_from_csv,',','.');
    }

} // namespace amt


#endif // AMT_PARSER_CSV_HPP
