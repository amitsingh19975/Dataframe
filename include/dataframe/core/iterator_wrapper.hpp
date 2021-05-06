#if !defined(AMT_DATAFRAME_CORE_ITERATOR_WRAPPER_HPP)
#define AMT_DATAFRAME_CORE_ITERATOR_WRAPPER_HPP

namespace amt{

    template<typename T>
    struct iterator_wrapper{

        constexpr iterator_wrapper(T begin, T end) noexcept
            : m_begin(begin)
            , m_end(end)
        {}

        constexpr decltype(auto) begin() noexcept{ return m_begin; }

        constexpr decltype(auto) end() noexcept{ return m_end; }

        constexpr decltype(auto) begin() const noexcept{ return m_begin; }

        constexpr decltype(auto) end() const noexcept{ return m_end; }

    private:
        T m_begin;
        T m_end;
    };

} // namespace amt


#endif // AMT_DATAFRAME_CORE_ITERATOR_WRAPPER_HPP
