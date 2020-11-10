#if !defined(AMT_DATAFRAME_FUNCTIONS_PRETTY_STRING_HPP)
#define AMT_DATAFRAME_FUNCTIONS_PRETTY_STRING_HPP

#include <dataframe/box.hpp>
#include <dataframe/traits/frame_traits.hpp>
#include <dataframe/traits/series_traits.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace amt {

namespace fn {

struct pretty_string_t {

    template <typename DataFrame>
    requires(Frame<DataFrame> || Series<DataFrame>) std::string
    operator()(DataFrame const &in, std::size_t indent = 4u,
               bool should_row_number_be_shown = true,
               std::size_t max_width = 30u, bool should_show_dtypes = true) {
        std::stringstream ss;
        m_row = should_row_number_be_shown;
        m_dtype = should_show_dtypes;
        max_width = std::max(30ul, max_width);
        sstream(ss, in, indent, max_width);
        return ss.str();
    }

    template <Frame FrameType>
    std::stringstream &sstream(std::stringstream &ss, FrameType const &in,
                               std::size_t indent = 4u,
                               std::size_t max_width = 30u) {
        max_width = std::max(30ul, max_width);
        auto rows = in.rows();
        auto cols = in.cols();
        auto sz = std::min(max_width, std::max(1ul, count_digit(rows)));

        std::vector<std::size_t> maxes(cols, 0u);
        std::vector<std::string> temp(cols * rows);

        for (auto i = 0u; i < cols; ++i){
            maxes[i] = std::min( max_width, std::max(in[i].name().size(), type_to_string(in[i]).size() + 4u) );
        }

        for (auto i = 0u; i < cols; ++i) {
            for (auto j = 0u; j < rows; ++j) {
                auto &str = temp[i * rows + j];
                convert_to_stringstream(in[i][j]);
                str = m_ss.str();
                maxes[i] = std::min(max_width, std::max(maxes[i], str.size()));
                m_ss.str("");
            }
        }

        auto sp = std::string(indent, ' ');
        process_header(in, maxes, sp, sz);
        for (auto j = 0u; j < rows; ++j) {
            m_ss << '\n';
            if (m_row) {
                m_ss << std::string(sz - count_digit(j), ' ') << j;
            }
            for (auto i = 0u; i < cols; ++i) {
                auto str = std::move(temp[i * rows + j]);
                add_eplison(str,maxes[i]);
                auto str_sz = maxes[i] < str.size() ? 0u : maxes[i] - str.size();
                if (m_row) {
                    m_ss << sp << std::string(str_sz, ' ')
                         << str;
                } else {
                    m_ss << std::string(str_sz, ' ') << str
                         << sp;
                }
            }
        }
        m_ss.swap(ss);
        return ss;
    }

    template <Series SeriesType>
    std::stringstream &sstream(std::stringstream &ss, SeriesType const &in,
                               std::size_t indent = 4u,
                               std::size_t max_width = 30u) {
        max_width = std::max(30ul, max_width);
        auto sz = count_digit(in.size());
        auto dtype = type_to_string(in);
        auto name_sz = std::min(max_width, std::max(in.name().size(), dtype.size() + 4ul));
        auto max = name_sz;
        std::vector<std::string> temp(in.size());
        auto i = 0u;

        add_eplison_to_type(dtype,max_width);

        for (auto const &el : in) {
            convert_to_stringstream(el);
            temp[i] = m_ss.str();
            max = std::min(max_width, std::max(max, temp[i].size()));
            m_ss.str("");
            ++i;
        }
        auto sp = std::string(indent, ' ');
        auto col_name = in.name();
        add_eplison(col_name, max_width);
        auto col_size = max < col_name.size() ? 0ul : max - col_name.size();
        auto d_size = max < (dtype.size() + 4ul) ? 0ul: max - (dtype.size() + 4ul);

        if (m_row) {
            m_ss << std::string(sz - 1u, ' ') << "#" << sp;
            m_ss << std::string(col_size, ' ') << col_name << '\n';
            m_ss << std::string(sz, ' ') << sp << std::string(d_size, ' ') << "[ " << dtype << " ]\n";
            m_ss << std::string(sz, '=') << sp << std::string(max, '=') << '\n';
        } else {
            m_ss << std::string(col_size, ' ') << col_name << '\n';
            m_ss << std::string(d_size, ' ') << "[ " << dtype << " ]\n";
            m_ss << std::string(max, '=') << '\n';
        }

        i = 0u;
        for (auto &el : temp) {
            auto dc = count_digit(i);
            add_eplison(el, max_width);
            auto el_size = max < el.size() ? 0ul : max - el.size();
            if (m_row) {
                m_ss << std::string(sz - dc, ' ') << i << sp;
            }
            m_ss << std::string(el_size, ' ') << std::move(el) << '\n';
            ++i;
        }
        m_ss.swap(ss);
        return ss;
    }

    constexpr pretty_string_t &operator<<(auto const &m) {
        m_ss << m;
        return *this;
    }

  private:
    
    std::string& add_eplison(std::string& el, std::size_t max_width) const{
        if( el.size() > max_width ){
            el = el.substr(0, max_width);
            auto last = el.back();
            if (last == '"') {
                el[max_width - 1] = '"';
                el[max_width - 2] = '.';
                el[max_width - 4] = '.';
            } else {
                el[max_width - 1] = '.';
                el[max_width - 2] = '.';
            }
            el[max_width - 3] = '.';
        }
        return el;
    }
    
    std::string& add_eplison_to_type(std::string& s, std::size_t max_width) const{
        if( s.size() > max_width - 4ul ){
            s = s.substr(0, max_width - 4u);
            s[s.size() - 1u] = '.';
            s[s.size() - 2u] = '.';
            s[s.size() - 3u] = '.';
        }
        return s;
    }

    template <typename T>
    requires(std::is_integral_v<T>) constexpr std::size_t
        count_digit(T val) const noexcept {
        if (val == T{})
            return 1u;
        std::size_t ct{};
        while (val) {
            ++ct;
            val /= 10;
        }
        return ct;
    }

    void convert_to_stringstream(Box auto const &el) {
        visit(el, [&el, this]<typename T>(T const &) {
            if constexpr (is_monostate_v<T>) {
                m_ss << "none";
            } else {
                m_ss << el;
            }
        });
    }

    void process_header(Frame auto const &in,
                        std::vector<std::size_t> const &maxes,
                        std::string const &sp, std::size_t sz) {
        auto cols = in.cols();

        if (m_row) {
            m_ss << std::string(sz - 1u, ' ') << '#';

            for (auto i = 0u; i < cols; ++i) {
                auto el = in[i].name();
                add_eplison(el,maxes[i]);
                auto el_size = maxes[i] < el.size() ? 0ul : maxes[i] - el.size();
                m_ss << sp << std::string(el_size, ' ') << el;
            }

            if( m_dtype ){
                m_ss << '\n' << std::string(sz, ' ');

                for (auto i = 0u; i < cols; ++i) {
                    auto el = type_to_string(in[i]);
                    add_eplison_to_type(el,maxes[i]);
                    auto el_size = maxes[i] < ( el.size() + 4ul ) ? 0ul : maxes[i] - ( el.size() + 4ul );
                    m_ss << sp << std::string(el_size, ' ') << "[ "<< el <<" ]";
                }
            }

            m_ss << '\n' << std::string(sz, '=') << sp;
        } else {
            
            for (auto i = 0u; i < cols; ++i) {
                auto el = in[i].name();
                add_eplison(el,maxes[i]);
                auto el_size = maxes[i] < el.size() ? 0ul : maxes[i] - el.size();
                m_ss << std::string(el_size, ' ') << el << sp;
            }

            if( m_dtype ){
                m_ss << '\n';

                for (auto i = 0u; i < cols; ++i) {
                    auto el = type_to_string(in[i]);
                    add_eplison_to_type(el,maxes[i]);
                    auto el_size = maxes[i] < ( el.size() + 4ul ) ? 0ul : maxes[i] - ( el.size() + 4ul );
                    m_ss << std::string(el_size, ' ') << "[ "<< el <<" ]" << sp;
                }
            }

            m_ss << '\n';
        }

        for (auto i = 0u; i < cols; ++i) {
            m_ss << std::string(maxes[i], '=') << sp;
        }
    }

  private:
    std::stringstream m_ss;
    bool m_row{true};
    bool m_dtype{true};
};

} // namespace fn

inline static auto pretty_string = fn::pretty_string_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_PRETTY_STRING_HPP
