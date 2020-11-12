#if !defined(AMT_DATAFRAME_CORE_ARGUMENT_LIST_HPP)
#define AMT_DATAFRAME_CORE_ARGUMENT_LIST_HPP

#include <dataframe/traits/basic_traits.hpp>
#include <dataframe/core/utils.hpp>
#include <array>

namespace amt{

    namespace detail{
        template<typename T, typename Tuple, std::size_t I = 0u>
        requires is_tuple_v<Tuple>
        constexpr auto find_type_tuple() noexcept{
            constexpr auto size = std::tuple_size_v<Tuple>;
            if constexpr( I >= size ){
                return std::integral_constant<std::size_t, size>{};
            }else{
                using el_type = std::tuple_element_t<I, Tuple>;
                constexpr auto is_same = std::is_same_v< std::decay_t<T>, std::decay_t<el_type> >;
                constexpr auto is_assignable = std::is_assignable_v<T, el_type>;
                constexpr auto is_copy_assignable = std::is_copy_assignable_v<T>;
                constexpr auto is_move_assignable = std::is_move_assignable_v<T>;

                if constexpr(is_same && (is_assignable || is_copy_assignable || is_move_assignable)){
                    return std::integral_constant<std::size_t, I>{};
                }else{
                    return find_type_tuple<T, Tuple, I + 1u>();
                }
            }
        }
    };

    template<typename Tuple, typename... Ts>
    requires is_tuple_v<Tuple>
    constexpr auto construct_arg_list(Tuple& tp, Ts&&... ts) noexcept{
        constexpr auto ts_size = sizeof...(Ts);
        constexpr auto size = std::tuple_size_v<Tuple>;
        std::array<bool, size > is_init;
        is_init.fill(false);

        auto ftp = std::forward_as_tuple(std::forward<Ts>(ts)...);
        static_for<ts_size>([&]<typename I>(I){
            using tuple_type = std::decay_t<decltype(ftp)>;
            using in_type = std::tuple_element_t<I::value,tuple_type>;
            using index_type = decltype(detail::find_type_tuple<in_type, Tuple>());
            constexpr auto idx = index_type::value;
            if constexpr(idx != size){
                is_init[idx] = true;
                std::get<idx>(tp) = std::move(std::get<I::value>(ftp));
            }
        });

        return is_init;
    }

    namespace arg{

        template<typename T>
        struct arg_base{
            constexpr arg_base(arg_base const &other) = default;
            constexpr arg_base(arg_base &&other) noexcept = default;
            constexpr arg_base &operator=(arg_base const &other) = default;
            constexpr arg_base &operator=(arg_base &&other) noexcept = default;
            constexpr ~arg_base() = default;

            constexpr arg_base() requires std::is_default_constructible_v<T> = default;
            constexpr arg_base(T const& val)
                : m_data(val)
            {}
            
            constexpr arg_base(T && val)
                : m_data(std::move(val))
            {}

            constexpr T const& value() const noexcept{
                return m_data;
            }

            constexpr T& value() noexcept{
                return m_data;
            }

        private:
            T m_data;
        };

    } // namespace arg

}

#endif // AMT_DATAFRAME_CORE_ARGUMENT_LIST_HPP
