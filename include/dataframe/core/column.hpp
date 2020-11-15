#if !defined(AMT_DATAFRAME_FUNCTIONS_COLUMN_HPP)
#define AMT_DATAFRAME_FUNCTIONS_COLUMN_HPP

#include <array>
#include <dataframe/core/utils.hpp>
#include <string>

namespace amt {

namespace fn {

template <std::size_t N> struct column_t {
    using string_type = std::string_view;
    using index_type = std::size_t;
    using value_type = std::variant<string_type, index_type>;
    using base_type = std::array<value_type, N>;

    template <typename... Args>
    constexpr column_t(Args &&... args) noexcept
        : m_cols({std::forward<Args>(args)...}) {}

    constexpr column_t(column_t const &other) noexcept = default;
    constexpr column_t(column_t &&other) noexcept = default;
    constexpr column_t &operator=(column_t const &other) noexcept = default;
    constexpr column_t &operator=(column_t &&other) noexcept = default;
    constexpr ~column_t() noexcept = default;

    constexpr bool has_index(index_type k) const {
        return std::holds_alternative<index_type>(m_cols[k]);
    }

    constexpr index_type index(index_type k) const {
        return std::get<index_type>(m_cols[k]);
    }

    constexpr string_type name(index_type k) const {
        return std::get<string_type>(m_cols[k]);
    }

    constexpr std::array<index_type, N> get_indices(Frame auto const& f) const{
        std::array<index_type, N> ids;
        for(auto i = 0u; i < N; ++i){
            if(has_index(i)) ids[i] = index(i);
            else ids[i] = f.name_index(name(i));
        }

        return ids;
    }

  private:
    base_type m_cols;
};

template <typename... Ts> column_t(Ts &&...) -> column_t<sizeof...(Ts)>;

template<typename T>
concept ColumnKey = std::is_constructible_v<std::string_view, T> || std::is_convertible_v<T,std::size_t>;

} // namespace fn

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_COLUMN_HPP
