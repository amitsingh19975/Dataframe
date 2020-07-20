#if !defined(AMT_PARSER_READER_HPP)
#define AMT_PARSER_READER_HPP

#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>

namespace amt::detail{
    
    struct reader{
        using base_type                 = std::ifstream;
        using int_type                  = typename base_type::int_type;
        using off_type                  = typename base_type::off_type;
        using pos_type                  = typename base_type::pos_type;
        using iterator                  = std::istream_iterator<char>;
        using reverse_iterator          = std::reverse_iterator<iterator>;

        reader() noexcept = default;
        reader(reader const& other) = delete;
        reader(reader&& other) noexcept = default;
        reader& operator=(reader const& other) = delete;
        reader& operator=(reader&& other) noexcept = default;

        ~reader() = default;

        reader(std::string_view filename, std::string_view sep = "\n")
            : m_file(filename.data(), std::ios::binary)
            , m_sep(std::move(sep))
        {
            if(!m_file){
                throw std::runtime_error(
                    "amt::detail::reader(std::string_view, std::string_view) : "
                    "file not found"
                );
            }
            std::sort(m_sep.begin(), m_sep.end());
        }

        inline iterator begin() noexcept{
            return iterator{m_file};
        }

        inline iterator end() noexcept{
            return iterator{};
        }

        inline reverse_iterator rbegin() noexcept{
            return std::make_reverse_iterator(end());
        }

        inline reverse_iterator rend() noexcept{
            return std::make_reverse_iterator(begin());
        }

        inline std::optional< std::string > try_next() {
            
            m_curr_sep = -1;

            if( has_next() ) return std::nullopt;

            std::string temp;

            while((m_curr_sep = next())){
                if( is_sep( static_cast<char>(m_curr_sep) ) || m_curr_sep == EOF) break;
                temp.push_back( static_cast<char>(m_curr_sep) );
                m_curr_sep = -1;
            }
            return temp;
        }

        inline bool next( std::string& res ) {
            auto temp = try_next();
            if( temp ){
                res += std::move(*temp);
                return true;
            }
            return false;
        }

        inline int_type next() {
            return m_file.get();
        }

        inline int_type prev() {
            seek(-1, std::ios::cur);
            return m_file.get();
        }

        inline void seek(pos_type pos){
            m_file.seekg(pos);
        }

        inline void skip(){
            (void)next();
        }

        inline void seek(off_type off, std::ios_base::seekdir way){
            m_file.seekg(off,way);
        }

        inline pos_type tell(){
            return m_file.tellg();
        }

        inline int_type peek(){
            return m_file.peek();
        }

        inline int_type peek(pos_type pos){
            auto curr = m_file.tellg();
            seek(pos,std::ios::cur);
            auto temp = m_file.get();
            seek(curr,std::ios::beg);
            return temp;
        }

        inline bool has_next(){
            return !m_file.fail();
        }

        inline constexpr char curr_sep() const noexcept{
            return static_cast<char>(m_curr_sep);
        }

        inline constexpr bool is_sep(char c) const noexcept{
            return std::binary_search(m_sep.begin(), m_sep.end(), c);
        }

    private:
        base_type   m_file;
        std::string m_sep;
        int_type    m_curr_sep{-1};
    };

} // namespace amt::detail


#endif // AMT_PARSER_READER_HPP
