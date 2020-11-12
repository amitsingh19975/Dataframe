#if !defined(AMT_DATAFRAME_CORE_COMPARE_HPP)
#define AMT_DATAFRAME_CORE_COMPARE_HPP

#include <dataframe/box_utils.hpp>
#include <functional>
#include <type_traits>

namespace amt {

namespace impl {

struct empty_fn{
    constexpr bool operator()(auto&&, auto&&) const noexcept{
        return false;
    }
};

template <typename LHS, typename Fn> struct compare_fn;

template <typename T>
struct is_compare_fn : std::false_type{};

template<typename T>
inline static constexpr auto is_compare_fn_v = is_compare_fn< std::decay_t<T> >::value;

template <typename LHS, typename Fn>
struct is_compare_fn< compare_fn<LHS,Fn> > : std::true_type{};

template <typename LHS, typename Fn> struct compare_fn {

    constexpr compare_fn(compare_fn const &other) noexcept = default;
    constexpr compare_fn(compare_fn &&other) noexcept = default;
    constexpr compare_fn &operator=(compare_fn const &other) noexcept = default;
    constexpr compare_fn &operator=(compare_fn &&other) noexcept = default;
    constexpr ~compare_fn() noexcept = default;

    constexpr compare_fn(LHS const &lhs) noexcept : m_lhs(lhs) {}

    constexpr compare_fn(Fn &&fn, LHS const &lhs) noexcept
        : m_lhs(lhs), m_fn(std::forward<Fn>(fn)) {}

    template <typename RHS> constexpr bool operator()(RHS const &rhs) const {
        using rhs_type = std::decay_t<RHS>;
        using lhs_type = std::decay_t<LHS>;
        if constexpr (Box<lhs_type> && Box<rhs_type>) {
            return std::invoke(m_fn, m_lhs, rhs);
        } else if constexpr (Box<lhs_type> && !Box<rhs_type>) {
            if constexpr (std::is_constructible_v<std::string, rhs_type>) {
                return std::invoke(m_fn, get<std::string>(m_lhs), rhs);
            } else {
                return std::invoke(m_fn, get<rhs_type>(m_lhs), rhs);
            }
        } else if constexpr (!Box<lhs_type> && Box<rhs_type>) {
            if constexpr (std::is_constructible_v<std::string, lhs_type>) {
                return std::invoke(m_fn, m_lhs, get<std::string>(rhs));
            } else {
                return std::invoke(m_fn, m_lhs, get<lhs_type>(rhs));
            }
        } else{
            if constexpr( is_compare_fn_v<lhs_type> ){
                return std::invoke(m_lhs, rhs);
            }else{
                return std::invoke(m_fn, m_lhs, rhs);
            }
        }
    }

  private:
    LHS const &m_lhs;
    Fn m_fn{};
};

template<typename LHS, typename RHS, typename OP>
struct fn_base{
    constexpr fn_base(fn_base const &other) noexcept = default;
    constexpr fn_base(fn_base &&other) noexcept = default;
    constexpr fn_base &operator=(fn_base const &other) noexcept = default;
    constexpr fn_base &operator=(fn_base &&other) noexcept = default;
    constexpr ~fn_base() noexcept = default;
    
    constexpr fn_base(LHS const& l, RHS const& r, OP const& op) noexcept
        : m_lhs(l)
        , m_rhs(r)
        , m_op(op)
    {}

    template<typename Value>
    constexpr decltype(auto) operator()(Value&& val) const {
        auto rhs = std::invoke(m_rhs, std::forward<Value>(val));
        auto lhs = std::invoke(m_lhs, std::forward<Value>(val));
        return std::invoke(m_op, lhs, rhs);
    }

private:
    LHS const& m_lhs;
    RHS const& m_rhs;
    OP const& m_op;
};

} // namespace impl

template <typename LHS> struct equal : impl::compare_fn<LHS, std::equal_to<>> {
    using base_type = impl::compare_fn<LHS, std::equal_to<>>;

    constexpr equal(LHS const &lhs) noexcept : base_type(lhs) {}
};

template <typename LHS>
struct not_equal : impl::compare_fn<LHS, std::not_equal_to<>> {
    using base_type = impl::compare_fn<LHS, std::not_equal_to<>>;

    constexpr not_equal(LHS const &lhs) noexcept : base_type(lhs) {}
};

template <typename LHS> struct less : impl::compare_fn<LHS, std::less<>> {
    using base_type = impl::compare_fn<LHS, std::less<>>;

    constexpr less(LHS const &lhs) noexcept : base_type(lhs) {}
};

template <typename LHS> struct greater : impl::compare_fn<LHS, std::greater<>> {
    using base_type = impl::compare_fn<LHS, std::greater<>>;

    constexpr greater(LHS const &lhs) noexcept : base_type(lhs) {}
};

template <typename LHS>
struct greater_eq : impl::compare_fn<LHS, std::greater_equal<>> {
    using base_type = impl::compare_fn<LHS, std::greater_equal<>>;

    constexpr greater_eq(LHS const &lhs) noexcept : base_type(lhs) {}
};

template <typename LHS>
struct less_eq : impl::compare_fn<LHS, std::less_equal<>> {
    using base_type = impl::compare_fn<LHS, std::less_equal<>>;

    constexpr less_eq(LHS const &lhs) noexcept : base_type(lhs) {}
};

template <typename LHS>
struct logical_and : impl::compare_fn<LHS, std::logical_and<>> {
    using base_type = impl::compare_fn<LHS, std::logical_and<>>;

    constexpr logical_and(LHS const &lhs) noexcept : base_type(lhs) {}
};

template <typename LHS>
struct logical_or : impl::compare_fn<LHS, std::logical_or<>> {
    using base_type = impl::compare_fn<LHS, std::logical_or<>>;

    constexpr logical_or(LHS const &lhs) noexcept : base_type(lhs) {}
};

} // namespace amt

template<typename L, typename R, typename OP, typename CLHS, typename CFN>
constexpr auto operator&&(amt::impl::fn_base<L, R, OP> const& l, amt::impl::compare_fn<CLHS, CFN> const& r) noexcept{
    return amt::impl::fn_base{ l, r, std::logical_and<>{} };
}

template<typename L, typename R, typename OP, typename CLHS, typename CFN>
constexpr auto operator&&( amt::impl::compare_fn<CLHS, CFN> const& l, amt::impl::fn_base<L,R,OP> const& r) noexcept{
    return amt::impl::fn_base{ l, r, std::logical_and<>{} };
}

template<typename LLHS, typename LFN, typename RLHS, typename RFN>
constexpr auto operator&&(amt::impl::compare_fn<LLHS, LFN> const& l, amt::impl::compare_fn<RLHS, RFN> const& r) noexcept{
    return amt::impl::fn_base{ l, r, std::logical_and<>{} };
}

template<typename L, typename R, typename OP, typename CLHS, typename CFN>
constexpr auto operator||(amt::impl::fn_base<L, R, OP> const& l, amt::impl::compare_fn<CLHS, CFN> const& r) noexcept{
    return amt::impl::fn_base{ l, r, std::logical_or<>{} };
}

template<typename L, typename R, typename OP, typename CLHS, typename CFN>
constexpr auto operator||( amt::impl::compare_fn<CLHS, CFN> const& l, amt::impl::fn_base<L,R,OP> const& r) noexcept{
    return amt::impl::fn_base{ l, r, std::logical_or<>{} };
}

template<typename LLHS, typename LFN, typename RLHS, typename RFN>
constexpr auto operator||(amt::impl::compare_fn<LLHS, LFN> const& l, amt::impl::compare_fn<RLHS, RFN> const& r) noexcept{
    return amt::impl::fn_base{ l, r, std::logical_or<>{} };
}

#endif // AMT_DATAFRAME_CORE_COMPARE_HPP
