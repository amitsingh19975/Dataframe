#if !defined(AMT_DATAFRAME_CORE_UNBOUNDED_STORAGE_HPP)
#define AMT_DATAFRAME_CORE_UNBOUNDED_STORAGE_HPP

#include <dataframe/core/traits/unbounded.hpp>
#include <dataframe/core/visitor.hpp>
#include <dataframe/core/iterator_wrapper.hpp>
#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <functional>
#include <string>
#include <optional>
#include <typeindex>

namespace amt{

    constexpr static std::ptrdiff_t NoUpperLimit = -1;
    
    template<typename S, std::ptrdiff_t TypesUpperBound = NoUpperLimit >
    class basic_unbounded_storage{
        static_assert(traits::StorageTrait<S>, "The first template argument, S must statisfy Storage Traits");
        static_assert(TypesUpperBound != 0, "Storage cannot hold any type, please set upper limit greater than 0 or set to NoUpperLimit");
        using storage_trait_type = S;

        struct function_table{
            using size_fn_t = std::function<std::size_t(basic_unbounded_storage const&)>;
            using copy_fn_t = std::function<void(basic_unbounded_storage&, basic_unbounded_storage const&)>;
            using move_fn_t = std::function<void(basic_unbounded_storage&, basic_unbounded_storage&)>;
            using destroy_fn_t = std::function<void(basic_unbounded_storage&)>;
            using equal_fn_t = std::function<bool(basic_unbounded_storage const&, basic_unbounded_storage const&)>;

            constexpr function_table() noexcept = default;

            function_table(
                    size_fn_t sz, copy_fn_t cp, 
                    move_fn_t mv, destroy_fn_t des,
                    equal_fn_t bin, std::type_index index
            ) noexcept
                : m_size_fn(std::move(sz))
                , m_copy_fn(std::move(cp))
                , m_move_fn(std::move(mv))
                , m_destroy_fn(std::move(des))
                , m_equal_fn(std::move(bin))
                , m_type_index(index)
            {}

            template<typename U>
            static constexpr function_table make() noexcept{
                return function_table(
                    [](basic_unbounded_storage const& self){ 
                        auto const& opt = m_data<U>[&self];
                        if(opt) return (*opt).size();
                        else return 0ul;
                    }
                    ,[](basic_unbounded_storage& self, basic_unbounded_storage const& other){ 
                        return m_data<U>[&self] = m_data<U>[&other]; 
                    }
                    ,[](basic_unbounded_storage & self, basic_unbounded_storage & other){ 
                        return m_data<U>[&self] = std::move(m_data<U>[&other]); 
                    }
                    ,[](basic_unbounded_storage& self){ 
                        return m_data<U>.erase(&self); 
                    }
                    ,[](basic_unbounded_storage const& self, basic_unbounded_storage const& other){ 
                        auto const& lopt = m_data<U>[&self];
                        auto const& ropt = m_data<U>[&other];
                        if(!lopt || !ropt) return false;
                        if((*lopt).size() != (*ropt).size()) return false;
                        return std::equal((*lopt).begin(), (*lopt).end(), (*ropt).begin());
                    },
                    {typeid(U)}
                );
            }

            constexpr auto copy(basic_unbounded_storage& self, basic_unbounded_storage const& other) const noexcept -> void{
                std::invoke(m_copy_fn, self, other);
            }

            constexpr auto move(basic_unbounded_storage& self, basic_unbounded_storage& other) const noexcept -> void{
                std::invoke(m_move_fn, self, other);
            }

            constexpr auto equal(basic_unbounded_storage const& self, basic_unbounded_storage const& other) const noexcept -> bool{
                return std::invoke(m_equal_fn, self, other);
            }

            constexpr auto size(basic_unbounded_storage const& self) const noexcept -> std::size_t{
                return std::invoke(m_size_fn, self);
            }

            constexpr auto destroy(basic_unbounded_storage& self) const noexcept -> void{
                std::invoke(m_destroy_fn, self);
            }

            template<typename T>
            auto is() const noexcept -> bool{
                return std::type_index(typeid(T)) == m_type_index;
            }

        private:
            size_fn_t m_size_fn{nullptr};
            copy_fn_t m_copy_fn{nullptr};
            move_fn_t m_move_fn{nullptr};
            destroy_fn_t m_destroy_fn{nullptr};
            equal_fn_t m_equal_fn{nullptr};
            std::type_index m_type_index;
        };

        template<typename T>
        using function_container = std::conditional_t<TypesUpperBound < 0, std::vector<T>, std::array<T, static_cast<std::size_t>(TypesUpperBound)> >;


    public:

        constexpr static bool has_upper_limit = TypesUpperBound >= 0;
        constexpr static bool is_static_storage = traits::StaticStorageTrait< storage_trait_type >;
        using size_type = typename storage_trait_type::size_type;
        using difference_type = typename storage_trait_type::difference_type;
        template<typename T>
        using storage_type = typename storage_trait_type::template rebind<T>;
        
        template<typename T>
        using optinal_storage_type = std::optional< storage_type<T> >;

        template<typename T>
        using base_type = std::unordered_map<basic_unbounded_storage const*, optinal_storage_type<T> >;
        
        constexpr basic_unbounded_storage() noexcept = default;
        constexpr basic_unbounded_storage(basic_unbounded_storage const&) = default;
        constexpr basic_unbounded_storage(basic_unbounded_storage &&) noexcept = default;

        constexpr basic_unbounded_storage& operator=(basic_unbounded_storage const& other){
            if(this == &other) return;
            m_fn_table = other.m_fn_table;
            clear();
            copy_helper(other);
        }
        constexpr basic_unbounded_storage& operator=(basic_unbounded_storage && other) noexcept{
            if(this == &other) return;
            m_fn_table = std::move(other.m_fn_table);
            clear();
            move_helper(other);
        }

        template<typename U>
        constexpr basic_unbounded_storage(storage_type<U> const& data){
            get_container<U>() = data;
        }

        template<typename U>
            requires is_static_storage
        constexpr basic_unbounded_storage(std::initializer_list<U> const& li){
            auto& temp = get_container<U>();
            if(temp.size() < li.size()){
                throw std::runtime_error(
                    "amt::basic_unbounded_storage(std::initializer_list<U> const&) : "
                    "the number of elements inside the initializer list exceeds the static container size"
                );
            }
            std::copy_n(li.begin(), li.end(), temp.begin());
        }

        template<typename U>
            requires (!is_static_storage)
        constexpr basic_unbounded_storage(std::initializer_list<U> const& li){
            get_container<U>() = std::move(li);
        }


        template<typename U>
            requires (!is_static_storage)
        constexpr basic_unbounded_storage(size_type sz, U def){
            get_container<U>() = storage_type<U>(sz,std::move(def));
        }

        constexpr ~basic_unbounded_storage() noexcept{
            clear();
        }

        template<typename T>
        constexpr auto data() -> optinal_storage_type<T>& { return get_container<T>(); }
        
        template<typename T>
        constexpr auto data() const -> optinal_storage_type<T> const& { return get_container<T>(); }

        constexpr auto size() const noexcept -> size_type{
            auto const sz = (has_upper_limit ? m_curr_pos : m_fn_table.size());
            size_type s{};
            for(auto i = 0ul; i < sz; ++i){
                s += m_fn_table[i].size(*this);
            }
            return s;
        };

        template<typename T>
        constexpr auto size() const -> size_type{
            auto const& opt = get_container<T>();
            return opt ? opt.value().size() : 0ul;
        };

        constexpr auto clear() noexcept -> void{
            auto const sz = (has_upper_limit ? m_curr_pos : m_fn_table.size());
            for(auto i = 0ul; i < sz; ++i){
                m_fn_table[i].destroy(*this);
            }
        }

        constexpr auto operator==(basic_unbounded_storage const& other) const noexcept -> bool{
            auto const sz = (has_upper_limit ? m_curr_pos : m_fn_table.size());
            for(auto i = 0ul; i < sz; ++i){
                if( !m_fn_table[i].equal(*this,other) ) return false;
            }
            return true;
        }

        constexpr auto operator!=(basic_unbounded_storage const& other) const noexcept -> bool{
            return !(*this == other);
        }

        template<typename T, typename... Ts>
        constexpr auto get_or_make(Ts&&... ts) noexcept -> optinal_storage_type<T>&{
            auto& opt = get_container<T>();
            if(!opt) opt = std::move(storage_type<T>(std::forward<Ts>(ts)...));
            return opt;
        }

    private:

        constexpr auto copy_helper(basic_unbounded_storage const& other) noexcept -> void{
            auto const sz = (has_upper_limit ? m_curr_pos : m_fn_table.size());
            
            for(auto i = 0ul; i < sz; ++i){
                m_fn_table[i].copy(*this, other);
            }
        }

        constexpr auto move_helper(basic_unbounded_storage& other) noexcept -> void{
            auto const sz = (has_upper_limit ? m_curr_pos : m_fn_table.size());
            
            for(auto i = 0ul; i < sz; ++i){
                m_fn_table[i].move(*this, other);
            }
        }

        template<typename T>
        constexpr auto get_container() -> optinal_storage_type<T>&{
            auto& item = m_data<T>;

            if(auto it = item.find(this); it == item.end()){
                push_fn<T>();
                return item[this] = std::nullopt;
            }else{
                return it->second;
            }
        }

        template<typename T>
        constexpr auto get_container() const -> optinal_storage_type<T> const&{
            auto const& item = m_data<T>;
            if(auto it = item.find(this); it == item.end()){
                throw std::runtime_error(
                    "amt::basic_unbounded_storage::get_container<T>() : "
                    "container with the type T does not exist, and const function cannot create the container."
                );
            }else{
                return it->second;
            }
        }

        template<typename T>
        constexpr auto push_fn() -> void requires has_upper_limit
        {
            if(m_curr_pos >= m_fn_table.size()){
                throw std::runtime_error(
                    "amt::basic_unbounded_storage::push_fn() : "
                    "types exceeds the set upper limit"
                );
            }
            m_fn_table[m_curr_pos++] = function_table::template make<T>();
        }

        template<typename T>
        constexpr auto push_fn() -> void requires (!has_upper_limit)
        {
            m_fn_table.emplace_back(function_table::template make<T>());
        }

    private:
        template<typename T, traits::UnboundedTypeStorage U>
        friend constexpr auto is(U&& s) noexcept -> bool;

        template<typename T>
        static base_type<T> m_data;

        function_container<function_table> m_fn_table{};
        
        std::size_t m_curr_pos{};
    };

    template<typename S,std::ptrdiff_t N>
    template<typename T>
    typename basic_unbounded_storage<S,N>::template base_type<T> basic_unbounded_storage<S,N>::m_data;


    using dynamic_unbounded_storage = basic_unbounded_storage<dynamic_storage<>>;

    template<std::size_t N>
    using static_unbounded_storage = basic_unbounded_storage<static_storage<N>>;

} // namespace amt

#endif // AMT_DATAFRAME_CORE_UNBOUNDED_STORAGE_HPP
