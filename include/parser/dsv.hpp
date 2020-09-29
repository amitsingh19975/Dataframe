#if !defined(AMT_PARSER_DSV_HPP)
#define AMT_PARSER_DSV_HPP

#include <parser/reader.hpp>
#include <frame.hpp>
#include <cctype>

namespace amt{
    
    namespace detail{
        
        template<typename Integer>
        requires std::is_integral_v<Integer>
        inline constexpr bool even(Integer num) noexcept{
            return (num & 1) == 0;
        }
        
        template<typename Integer>
        requires std::is_integral_v<Integer>
        inline constexpr bool odd(Integer num) noexcept{
            return !even(num);
        }

        inline bool parse_quotes( reader& rd, std::string& field, std::size_t& qcount ){
            while( rd.peek() == '"' && rd.has_next() ) {
                ++qcount;
                if( ( qcount > 1 ) && odd(qcount) ){
                    field += '"';
                }
                rd.skip();
                while( odd(rd.peek()) && std::isspace(rd.peek()) && rd.has_next() ) rd.skip();
            }
            
            if( even(qcount) ) {
                return qcount;
            }
            return false;
        }

        inline bool parse_field( reader& rd, std::string& field, char sep, char dec ){
            std::size_t qcount = 0;

            while( rd.has_next() ){

                if( ( rd.peek() == sep ) && even(qcount)) {
                    rd.skip();
                    break;
                }

                if( ( rd.peek() == '"' ) && parse_quotes(rd, field, qcount) && !rd.is_sep( static_cast<char>(rd.peek()) ) ) {
                    qcount = 0;
                    continue;
                }

                if( even(qcount) && rd.is_sep( static_cast<char>(rd.peek()) ) ){
                    rd.skip();
                    return true;
                }

                if( rd.has_next() ){
                    char c =  static_cast<char>(rd.next());
                    if( even(qcount) && ( c == dec ) ){
                        c = '.';
                    }
                    field.push_back(c);
                }else{
                    rd.next();
                }

            }

            if ( odd(qcount) ){
                throw std::runtime_error(
                    "amt::detail::parse_field(reader&, std::string&, char) : "
                    "No matching quotes were found"
                );
            }

            return false;
        }

        inline void ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
            }));
        }

        inline void rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), s.end());
        }

        inline void trim(std::string &s) {
            ltrim(s);
            rtrim(s);
        }

            
        template<typename... Ts>
        inline auto read(std::string_view filename, char sep, char dec, bool use_header_from_csv = false){
            using ret_type = frame<Ts...>;
            
            std::vector<std::string> temp;
            temp.push_back("");

            std::size_t i{};

            reader r(filename);

            while( r.has_next() && !detail::parse_field(r,temp[i++], sep, dec) ){
                temp.push_back("");
            }
            if( i == 1 && temp[0].empty() ) return ret_type{};
            
            ret_type res(i);

            if( use_header_from_csv ){
                res.set_name(std::move(temp));
            }else{
                std::size_t j{};
                for(auto& el : temp){
                    detail::trim(el);
                    res[j++].push_back( std::move(el) );
                }
            }

            temp.resize(i);
            
            while( r.has_next() ){
                i = 0;
                while( r.has_next() && !detail::parse_field(r,temp[i++], sep, dec) ){}
                
                if( i == 1 && temp[0].empty() ) break;

                if ( i != res.cols() ){
                    throw std::runtime_error(
                        "boost::numeric::ublas::detail::read(std::string_view, char, bool) : "
                        "invalid format"
                    );
                }

                std::size_t j{};
                for(auto& el : temp){
                    detail::trim(el);
                    res[j++].push_back( std::move(el) );
                }
            }

            return res;
        }

    } // namespace detail
    
    template<typename... Ts>
    inline auto read_dsv(std::string_view filename, bool use_header_from_dsv, char sep, char dec){
        return detail::read<Ts...>(filename,sep, dec, use_header_from_dsv);
    }

} // namespace amt


#endif // AMT_PARSER_DSV_HPP
