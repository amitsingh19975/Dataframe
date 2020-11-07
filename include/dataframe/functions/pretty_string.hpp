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
               std::size_t max_width = 30u) {
        std::stringstream ss;
        m_row = should_row_number_be_shown;
        sstream(ss, in, indent, max_width);
        return ss.str();
    }

    template <Frame FrameType>
    std::stringstream &sstream(std::stringstream &ss, FrameType const &in,
                               std::size_t indent = 4u,
                               std::size_t max_width = 30u) {
        auto rows = in.rows();
        auto cols = in.cols();
        auto sz = std::min(max_width, std::max(1ul, count_digit(rows)));

        std::vector<std::size_t> maxes(cols, 0u);
        std::vector<std::string> temp(cols * rows);

        for (auto i = 0u; i < cols; ++i)
            maxes[i] = in[i].name().size();

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
                if (str.size() > maxes[i]) {
                    auto last = str.back();
                    str = str.substr(0, maxes[i]);
                    if (last == '"') {
                        str[maxes[i] - 1] = '"';
                        str[maxes[i] - 2] = '.';
                        str[maxes[i] - 4] = '.';
                    } else {
                        str[maxes[i] - 1] = '.';
                        str[maxes[i] - 2] = '.';
                    }
                    str[maxes[i] - 3] = '.';
                }
                if (m_row) {
                    m_ss << sp << std::string(maxes[i] - str.size(), ' ')
                         << str;
                } else {
                    m_ss << std::string(maxes[i] - str.size(), ' ') << str
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
        auto sz = count_digit(in.size());
        auto name_sz = std::min(max_width, in.name().size());
        auto max = name_sz;
        std::vector<std::string> temp(in.size());
        auto i = 0u;

        for (auto const &el : in) {
            convert_to_stringstream(el);
            temp[i] = m_ss.str();
            max = std::min(max_width, std::max(max, temp[i].size()));
            m_ss.str("");
            ++i;
        }
        auto sp = std::string(indent, ' ');
        if (m_row) {
            m_ss << std::string(sz - 1u, ' ') << "#" << sp;
            m_ss << std::string(max - name_sz, ' ') << in.name() << '\n';
            m_ss << std::string(sz, '=') << sp << std::string(max, '=') << '\n';
        } else {
            m_ss << std::string(max - name_sz, ' ') << in.name() << '\n';
            m_ss << std::string(max, '=') << '\n';
        }

        i = 0u;
        for (auto &el : temp) {
            auto dc = count_digit(i);
            if (el.size() > max) {
                auto last = el.back();
                el = el.substr(0, max);
                if (last == '"') {
                    el[max - 1] = '"';
                    el[max - 2] = '.';
                    el[max - 4] = '.';
                } else {
                    el[max - 1] = '.';
                    el[max - 2] = '.';
                }
                el[max - 3] = '.';
            }
            if (m_row) {
                m_ss << std::string(sz - dc, ' ') << i << sp;
            }
            m_ss << std::string(max - el.size(), ' ') << std::move(el) << '\n';
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
                auto &el = in[i].name();
                m_ss << sp << std::string(maxes[i] - el.size(), ' ') << el;
            }

            m_ss << '\n' << std::string(sz, '=') << sp;
        } else {
            for (auto i = 0u; i < cols; ++i) {
                auto &el = in[i].name();
                m_ss << std::string(maxes[i] - el.size(), ' ') << el << sp;
            }
            m_ss << '\n';
        }

        for (auto i = 0u; i < cols; ++i) {
            m_ss << std::string(maxes[i], '=') << sp;
        }
    }

  private:
    std::stringstream m_ss;
    bool m_row{false};
};

} // namespace fn

inline static auto pretty_string = fn::pretty_string_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_PRETTY_STRING_HPP
