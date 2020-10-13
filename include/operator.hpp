#if !defined(AMT_CORE_OPERATOR_HPP)
#define AMT_CORE_OPERATOR_HPP

#include <core/type_to_string.hpp>
#include <core/type_traits.hpp>
#include <functional>
#include <iomanip>
#include <ostream>
#include <core/result.hpp>

namespace amt::detail {

template <typename T> inline constexpr auto size(T &&v) noexcept {
    using ret_type = std::decay_t<T>;
    if constexpr (is_frame_v<ret_type> || is_frame_view_v<T>) {
        return v.cols();
    } else {
        return v.size();
    }
}

template <typename T> inline constexpr auto cols(T &&v) noexcept {
    using ret_type = std::decay_t<T>;
    if constexpr (is_frame_v<ret_type> || is_frame_view_v<T>) {
        return v.cols();
    } else {
        return 1ul;
    }
}

template <typename T> inline constexpr auto rows(T &&v) noexcept {
    using ret_type = std::decay_t<T>;
    if constexpr (is_frame_v<ret_type> || is_frame_view_v<T>) {
        return v.rows();
    } else {
        return v.size();
    }
}

} // namespace amt::detail

namespace amt::core {

template <typename Container1, typename Container2, typename BinaryFn>
requires(FrameViewOrFrame<Container1> ||
         SeriesViewOrSeries<
             Container2>) inline constexpr result<bool> equal(Container1 const &LHS,
                                                      Container2 const &RHS,
                                                      BinaryFn &&fn) noexcept {
    std::size_t sz = ::amt::detail::size(LHS);
    
    bool f = true;
#pragma omp parallel for schedule(static)
    for (auto i = 0ul; i < sz; ++i) {
        auto res = std::invoke(fn,LHS[i],RHS[i]);
        using res_type = std::decay_t<decltype(res)>;
        if constexpr( is_result_v<res_type> ){
            f = f && res.value_or(false);
        }else{
            f = f && res;
        }
        
    }
    return {f};
}

} // namespace amt::core

namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os, Box auto &&s) {
    if (s.empty())
        return os;
    visit(s, [&os](auto &&val) { os << val; });
    return os;
}

inline constexpr result<bool> operator==(Box auto &&LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        return {false};
    }

    if (LHS.index() != RHS.index()) {
        return {false};
    }
    result<bool> f = false;
    visit(LHS, [&RHS, &f](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if constexpr (!core::has_comp_equal_v<ltype, rtype>) f = error_code::OP_EQ_EQ;
            else{
                using common_type = std::common_type_t<rtype,ltype>;
                f = (static_cast<common_type>(lval) == static_cast<common_type>(rval));
            }
        });
    });

    return f;
}

inline constexpr result<bool> operator<(Box auto &&LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        return {false};
    }

    if (LHS.index() != RHS.index()) {
        return {false};
    }
    result<bool> f = false;
    visit(LHS, [&RHS, &f](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if constexpr (!core::has_comp_less_v<ltype, rtype>) f = error_code::OP_LESS;
            else{
                using common_type = std::common_type_t<rtype,ltype>;
                f = (static_cast<common_type>(lval) < static_cast<common_type>(rval));
            }
        });
    });

    return f;
}

inline constexpr result<bool> operator>(Box auto &&LHS, Box auto &&RHS) {
    return RHS < LHS;
}

inline constexpr bool operator!=(Box auto &&LHS, Box auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr auto operator+(Box auto &&LHS, Box auto &&RHS) {
    using return_type = std::decay_t<decltype(LHS)>;

    if (LHS.empty() || RHS.empty()) {
        return result<return_type>{ error_code::EMPTY };
    }

    if (LHS.index() != RHS.index()) {
        return result<return_type>{ error_code::TYPE_MISMATCH };
    }

    result<return_type> temp;
    
    visit(LHS, [&RHS,&temp](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if constexpr (!core::has_op_plus_v<ltype, rtype>) temp = error_code::OP_PLUS;
            else{
                using common_type = std::common_type_t<rtype,ltype>;
                return_type res = static_cast<common_type>(lval) + static_cast<common_type>(rval);
                temp = std::move(res);
            }
        });
    });

    return temp;
}

inline constexpr auto operator-(Box auto &&LHS, Box auto &&RHS) {
    using return_type = std::decay_t<decltype(LHS)>;

    if (LHS.empty() || RHS.empty()) {
        return result<return_type>{ error_code::EMPTY };
    }

    if (LHS.index() != RHS.index()) {
        return result<return_type>{ error_code::TYPE_MISMATCH };
    }
    
    result<return_type> temp;

    visit(LHS, [&RHS,&temp](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if constexpr (!core::has_op_minus_v<ltype, rtype>) temp = error_code::OP_MINUS;
            else{
                using common_type = std::common_type_t<rtype,ltype>;
                return_type res = static_cast<common_type>(lval) - static_cast<common_type>(rval);
                temp = std::move(res);
            }
        });
    });

    return temp;
}

inline constexpr auto operator*(Box auto &&LHS, Box auto &&RHS) {
    using return_type = std::decay_t<decltype(LHS)>;

    if (LHS.empty() || RHS.empty()) {
        return result<return_type>{ error_code::EMPTY };
    }

    if (LHS.index() != RHS.index()) {
        return result<return_type>{ error_code::TYPE_MISMATCH };
    }
    
    result<return_type> temp;

    visit(LHS, [&RHS,&temp](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if constexpr (!core::has_op_mul_v<ltype, rtype>) temp = error_code::OP_MUL;
            else{
                using common_type = std::common_type_t<rtype,ltype>;
                return_type res = static_cast<common_type>(lval) * static_cast<common_type>(rval);
                temp = std::move(res);
            }
        });
    });

    return temp;
}

inline constexpr auto operator/(Box auto &&LHS, Box auto &&RHS) {
    using return_type = std::decay_t<decltype(LHS)>;

    if (LHS.empty() || RHS.empty()) {
        return result<return_type>{ error_code::EMPTY };
    }

    if (LHS.index() != RHS.index()) {
        return result<return_type>{ error_code::TYPE_MISMATCH };
    }
    
    result<return_type> temp;

    visit(LHS, [&RHS,&temp](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if constexpr (!core::has_op_div_v<ltype, rtype>) temp = error_code::OP_DIV;
            else{
                using common_type = std::common_type_t<rtype,ltype>;
                return_type res = static_cast<common_type>(lval) / static_cast<common_type>(rval);
                temp = std::move(res);
            }
        });
    });

    return temp;
}

} // namespace amt

namespace amt {

inline constexpr result<bool> operator<=(Box auto &&LHS, Box auto &&RHS) {
    return (LHS < RHS) && (LHS == RHS);
}

inline constexpr result<bool> operator>=(Box auto &&LHS, Box auto &&RHS) {
    return (LHS > RHS) && (LHS == RHS);
}

inline constexpr auto &operator+=(Box auto &LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator+=(Box auto&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator+=(Box auto&, Box auto&&) : "
                                 "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if (!core::has_op_plus_equal_v<ltype, rtype>)
                throw std::runtime_error(
                    "amt::operator+=(Box auto&, Box auto&&) : "
                    "Operator (+=) is not supported by this type");
            else{
                if constexpr (core::has_op_plus_equal_v<ltype, rtype>) {
                    LHS.template as<ltype>() += rval;
                }
            }
        });
    });

    return LHS;
}

inline constexpr auto &operator-=(Box auto &LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator-=(Box auto&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator-=(Box auto&, Box auto&&) : "
                                 "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if (!core::has_op_minus_equal_v<ltype, rtype>)
                throw std::runtime_error(
                    "amt::operator-=(Box auto&, Box auto&&) : "
                    "Operator (-=) is not supported by this type");
            else{
                if constexpr (core::has_op_minus_equal_v<ltype, rtype>) {
                    LHS.template as<ltype>() -= rval;
                }
            }
        });
    });

    return LHS;
}

inline constexpr auto &operator/=(Box auto &LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator/=(Box auto&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator/=(Box auto&, Box auto&&) : "
                                 "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if (!core::has_op_div_equal_v<ltype, rtype>)
                throw std::runtime_error(
                    "amt::operator/=(Box auto&, Box auto&&) : "
                    "Operator (/=) is not supported by this type");
            else{
                if constexpr (core::has_op_div_equal_v<ltype, rtype>) {
                    LHS.template as<ltype>() /= rval;
                }
            }
        });
    });

    return LHS;
}

inline constexpr auto &operator*=(Box auto &LHS, Box auto &&RHS) {
    if (LHS.empty() || RHS.empty()) {
        throw std::runtime_error("amt::operator*=(Box auto&, Box auto&&) : "
                                 "LHS or RHS is empty");
    }

    if (LHS.index() != RHS.index()) {
        throw std::runtime_error("amt::operator*=(Box auto&, Box auto&&) : "
                                 "type mismatch");
    }

    visit(LHS, [&RHS, &LHS](auto &&lval) {
        visit(RHS, [&](auto &&rval) {
            using rtype = std::decay_t<decltype(rval)>;
            using ltype = std::decay_t<decltype(lval)>;
            if (!core::has_op_mul_equal_v<ltype, rtype>)
                throw std::runtime_error(
                    "amt::operator*=(Box auto&, Box auto&&) : "
                    "Operator (*=) is not supported by this type");
            else{
                if constexpr (core::has_op_mul_equal_v<ltype, rtype>) {
                    LHS.template as<ltype>() *= rval;
                }
            }
        });
    });

    return LHS;
}

} // namespace amt

// SeriesViewOrSeries operators
namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os,
                                          SeriesViewOrSeries auto &&s) {
    if (s.empty())
        return os << "( Col: \"\", Type: None )[]";
    std::size_t i{};
    os << "( Col: " << std::quoted(s.name()) << ", Type: " << type_to_string(s)
       << " )[ ";
    for (auto const &el : s) {
        if (el.is_string()) {
            os << std::quoted(el.template as<std::string>());
        } else if (el.is_char()) {
            os << "'" << el << "'";
        } else {
            os << el;
        }
        if (i++ < s.size() - 1ul) {
            os << ", ";
        }
    }
    return os << " ]";
}

inline constexpr bool operator==(SeriesViewOrSeries auto &&LHS,
                                 SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        return false;
    }
    return core::equal(LHS, RHS,
                       [](auto const &l, auto const &r) { return l == r; }).value_or(false);
}

inline constexpr bool operator!=(SeriesViewOrSeries auto &&LHS,
                                 SeriesViewOrSeries auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr result<bool> operator<(SeriesViewOrSeries auto &&LHS,
                                SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        return {error_code::SIZE_MISMATCH};
    }
    return core::equal(LHS, RHS, std::less<>{});
}

inline constexpr result<bool> operator<=(SeriesViewOrSeries auto &&LHS,
                                 SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        return {error_code::SIZE_MISMATCH};
    }
    return core::equal(LHS, RHS, std::less_equal<>{});
}

inline constexpr result<bool> operator>(SeriesViewOrSeries auto &&LHS,
                                SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        return {error_code::SIZE_MISMATCH};
    }
    return core::equal(LHS, RHS, std::greater<>{});
}

inline constexpr result<bool> operator>=(SeriesViewOrSeries auto &&LHS,
                                 SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        return {error_code::SIZE_MISMATCH};
    }
    return core::equal(LHS, RHS, std::greater_equal<>{});
}

inline auto operator+(SeriesViewOrSeries auto &&LHS,
                      SeriesViewOrSeries auto &&RHS) {
    using ret_type = result_type_t<decltype(LHS)>;
    
    if (LHS.size() != RHS.size()) {
        return result<ret_type>{error_code::SIZE_MISMATCH};
    }
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());
    
    bool err_f = false;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        auto res = LHS[i] + RHS[i];
        err_f |= res.has_err();
        if(res) temp[i] = *res;
    }
    if( !err_f )
        return result<ret_type>( std::move(temp) );
    else
        return result<ret_type>{error_code::OP_PLUS};
}

inline auto operator-(SeriesViewOrSeries auto &&LHS,
                      SeriesViewOrSeries auto &&RHS) {
    using ret_type = result_type_t<decltype(LHS)>;
    
    if (LHS.size() != RHS.size()) {
        return result<ret_type>{error_code::SIZE_MISMATCH};
    }
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());
    
    bool err_f = false;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        auto res = LHS[i] - RHS[i];
        err_f |= res.has_err();
        if(res) temp[i] = *res;
    }
    
    if( !err_f )
        return result<ret_type>( std::move(temp) );
    else
        return result<ret_type>{error_code::OP_MINUS};
}

inline auto operator*(SeriesViewOrSeries auto &&LHS,
                      SeriesViewOrSeries auto &&RHS) {
    using ret_type = result_type_t<decltype(LHS)>;
    
    if (LHS.size() != RHS.size()) {
        return result<ret_type>{error_code::SIZE_MISMATCH};
    }
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());
    
    bool err_f = false;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        auto res = LHS[i] * RHS[i];
        err_f |= res.has_err();
        if(res) temp[i] = *res;
    }
    
    if( !err_f )
        return result<ret_type>( std::move(temp) );
    else
        return result<ret_type>{error_code::OP_MUL};
}

inline auto operator/(SeriesViewOrSeries auto &&LHS,
                      SeriesViewOrSeries auto &&RHS) {
    using ret_type = result_type_t<decltype(LHS)>;
    
    if (LHS.size() != RHS.size()) {
        return result<ret_type>{error_code::SIZE_MISMATCH};
    }
    using size_type = typename ret_type::size_type;

    ret_type temp(LHS.size());
    
    bool err_f = false;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        auto res = LHS[i] / RHS[i];
        err_f |= res.has_err();
        if(res) temp[i] = *res;
    }
    
    if( !err_f )
        return result<ret_type>( std::move(temp) );
    else
        return result<ret_type>{error_code::OP_DIV};
}

inline auto &operator+=(SeriesViewOrSeries auto &LHS,
                        SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator+=(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] += RHS[i];
    }
    return LHS;
}

inline auto &operator-=(SeriesViewOrSeries auto &LHS,
                        SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator-=(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] -= RHS[i];
    }
    return LHS;
}

inline auto &operator*=(SeriesViewOrSeries auto &LHS,
                        SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator*=(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] *= RHS[i];
    }
    return LHS;
}

inline auto &operator/=(SeriesViewOrSeries auto &LHS,
                        SeriesViewOrSeries auto &&RHS) {
    if (LHS.size() != RHS.size()) {
        throw std::runtime_error(
            "amt::operator/=(SeriesViewOrSeries&&, SeriesViewOrSeries&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.size(); ++i) {
        LHS[i] /= RHS[i];
    }
    return LHS;
}

} // namespace amt

// Slice
namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os, Slice auto &&s) {
    return os << "[ Start: " << s.first() << ", End: " << s.last()
              << ", Stride: " << s.step() << " ]";
}

} // namespace amt

namespace amt {

inline constexpr std::ostream &operator<<(std::ostream &os,
                                          FrameViewOrFrame auto &&f) {
    if (f.empty())
        return os << "{}";

    os << "{ \n";
    auto sp = "    ";
    for (auto const &s : f) {
        os<<sp<<s<<'\n';
    }
    os << "}\n";
    return os;
}

inline constexpr bool operator==(FrameViewOrFrame auto &&LHS,
                                 FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        return false;
    }
    return core::equal(LHS, RHS,
                       [](auto const &l, auto const &r) { return l == r; }).value_or(false);
}

inline constexpr bool operator!=(FrameViewOrFrame auto &&LHS,
                                 FrameViewOrFrame auto &&RHS) {
    return !(LHS == RHS);
}

inline constexpr result<bool> operator<(FrameViewOrFrame auto &&LHS,
                                FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        return {error_code::SIZE_MISMATCH};
    }
    return core::equal(LHS, RHS, std::less<>{});
}

inline constexpr result<bool> operator<=(FrameViewOrFrame auto &&LHS,
                                 FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        return {error_code::SIZE_MISMATCH};
    }
    return core::equal(LHS, RHS, std::less_equal<>{});
}

inline constexpr result<bool> operator>(FrameViewOrFrame auto &&LHS,
                                FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        return {error_code::SIZE_MISMATCH};
    }
    return core::equal(LHS, RHS, std::greater<>{});
}

inline constexpr result<bool> operator>=(FrameViewOrFrame auto &&LHS,
                                 FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        return {error_code::SIZE_MISMATCH};
    }
    return core::equal(LHS, RHS, std::greater_equal<>{});
}

inline auto operator+(FrameViewOrFrame auto &&LHS,
                      FrameViewOrFrame auto &&RHS) {
    
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    if (LHS.cols() != RHS.cols()) {
        return result<ret_type>{error_code::SIZE_MISMATCH};
    }

    ret_type temp(LHS.cols());
    
    bool err_f = false;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        auto res = LHS[i] + RHS[i];
        err_f |= res.has_err();
        if(res) temp[i] = *res;
    }
    
    if( !err_f )
        return result<ret_type>( std::move(temp) );
    else
        return result<ret_type>{error_code::OP_PLUS};
}

inline auto operator-(FrameViewOrFrame auto &&LHS,
                      FrameViewOrFrame auto &&RHS) {
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    if (LHS.cols() != RHS.cols()) {
        return result<ret_type>{error_code::SIZE_MISMATCH};
    }

    ret_type temp(LHS.cols());
    
    bool err_f = false;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        auto res = LHS[i] - RHS[i];
        err_f |= res.has_err();
        if(res) temp[i] = *res;
    }
    
    if( !err_f )
        return result<ret_type>( std::move(temp) );
    else
        return result<ret_type>{error_code::OP_MINUS};
}

inline auto operator*(FrameViewOrFrame auto &&LHS,
                      FrameViewOrFrame auto &&RHS) {
    
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    if (LHS.cols() != RHS.cols()) {
        return result<ret_type>{error_code::SIZE_MISMATCH};
    }

    ret_type temp(LHS.cols());
    
    bool err_f = false;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        auto res = LHS[i] * RHS[i];
        err_f |= res.has_err();
        if(res) temp[i] = *res;
    }
    
    if( !err_f )
        return result<ret_type>( std::move(temp) );
    else
        return result<ret_type>{error_code::OP_MUL};
}

inline auto operator/(FrameViewOrFrame auto &&LHS,
                      FrameViewOrFrame auto &&RHS) {
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

    if (LHS.cols() != RHS.cols()) {
        return result<ret_type>{error_code::SIZE_MISMATCH};
    }

    ret_type temp(LHS.cols());
    
    bool err_f = false;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        auto res = LHS[i] / RHS[i];
        err_f |= res.has_err();
        if(res) temp[i] = *res;
    }
    
    if( !err_f )
        return result<ret_type>( std::move(temp) );
    else
        return result<ret_type>{error_code::OP_DIV};
}

inline auto &operator+=(FrameViewOrFrame auto &LHS,
                        FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator+=(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] += RHS[i];
    }
    return LHS;
}

inline auto &operator-=(FrameViewOrFrame auto &LHS,
                        FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator-=(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] -= RHS[i];
    }
    return LHS;
}

inline auto &operator*=(FrameViewOrFrame auto &LHS,
                        FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator*=(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] *= RHS[i];
    }
    return LHS;
}

inline auto &operator/=(FrameViewOrFrame auto &LHS,
                        FrameViewOrFrame auto &&RHS) {
    if (LHS.cols() != RHS.cols()) {
        throw std::runtime_error(
            "amt::operator/=(FrameViewOrFrame&&, FrameViewOrFrame&&) : "
            "size mismatch");
    }
    using ret_type = result_type_t<decltype(LHS)>;
    using size_type = typename ret_type::size_type;

#pragma omp parallel for schedule(static)
    for (auto i = size_type{}; i < LHS.cols(); ++i) {
        LHS[i] /= RHS[i];
    }
    return LHS;
}

} // namespace amt

#endif // AMT_CORE_OPERATOR_HPP
