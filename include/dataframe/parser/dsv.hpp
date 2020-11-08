#if !defined(AMT_PARSER_DSV_HPP)
#define AMT_PARSER_DSV_HPP

#include <cctype>
#include <dataframe.hpp>
#include <dataframe/parser/reader.hpp>

namespace amt {

namespace detail {

template <typename Integer>
requires std::is_integral_v<Integer> inline constexpr bool
even(Integer num) noexcept {
    return (num & 1) == 0;
}

template <typename Integer>
requires std::is_integral_v<Integer> inline constexpr bool
odd(Integer num) noexcept {
    return !even(num);
}

inline bool parse_quotes(reader &rd, std::string &field, std::size_t &qcount) {
    while (rd.peek() == '"' && rd.has_next()) {
        ++qcount;
        if ((qcount > 1) && odd(qcount)) {
            field += '"';
        }
        rd.skip();
        while (odd(rd.peek()) && std::isspace(rd.peek()) && rd.has_next())
            rd.skip();
    }

    if (even(qcount)) {
        return qcount;
    }
    return false;
}

inline bool parse_field(reader &rd, std::string &field, char sep, char dec) {
    std::size_t qcount = 0;

    if (rd.parsed()) {
        rd.parsed(false);
        return true;
    }

    while (rd.has_next()) {

        if ((rd.peek() == sep) && even(qcount)) {
            rd.skip();
            break;
        }

        if ((rd.peek() == '"') && parse_quotes(rd, field, qcount) &&
            !rd.is_sep(static_cast<char>(rd.peek()))) {
            qcount = 0;
            continue;
        }

        if (even(qcount) && rd.is_sep(static_cast<char>(rd.peek()))) {
            rd.parsed(true);
            rd.skip();
            return false;
        }

        if (rd.has_next()) {
            char c = static_cast<char>(rd.next());
            if (even(qcount) && (c == dec)) {
                c = '.';
            }
            field.push_back(c);
        } else {
            rd.next();
        }
    }

    if (odd(qcount)) {
        throw std::runtime_error(
            "amt::detail::parse_field(reader&, std::string&, char) : "
            "No matching quotes were found");
    }

    if (field.empty() && !rd.has_next()) {
        return true;
    }

    return false;
}

inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](int ch) { return !std::isspace(ch); }));
}

inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

template <Box B = box>
inline auto read(std::string_view filename, char sep, char dec,
                 bool use_header_from_csv = false) {
    using ret_type = basic_frame<B>;
    using series_type = basic_series<B>;

    std::string temp;
    series_type temp_ser{dtype<std::string>{}};

    std::size_t i{};

    reader r(filename);
    auto lines = r.lines();

    while (r.has_next() && !detail::parse_field(r, temp, sep, dec)) {
        detail::trim(temp);
        temp_ser.push_back(std::move(temp));
        ++i;
    }

    if (i == 1 && get<std::string>(temp_ser[0]).empty())
        return ret_type{};

    ret_type res(i);
    res.dtype(dtype<std::string>());

    res.reserve(i, lines);

    if (use_header_from_csv) {
        res.set_names(std::move(temp_ser));
    } else {
        res.row_push_back(std::move(temp_ser));
    }

    while (r.has_next()) {
        i = 0;
        while (r.has_next() && !detail::parse_field(r, temp, sep, dec)) {
            if (i >= res.cols()) {
                throw std::runtime_error(
                    "amt::detail::read(std::string_view, char, bool) : "
                    "invalid format");
            }
            detail::trim(temp);
            res[i++].push_back(std::move(temp));
        }
    }

    return res;
}

} // namespace detail

template <Box B = box>
inline auto read_dsv(std::string_view filename, bool use_header_from_dsv,
                     char sep, char dec) {
    return detail::read<B>(filename, sep, dec, use_header_from_dsv);
}

} // namespace amt

#endif // AMT_PARSER_DSV_HPP
