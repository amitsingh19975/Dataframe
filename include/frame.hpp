#if !defined(AMT_FRAME_HPP)
#define AMT_FRAME_HPP

#include <frame_view.hpp>
#include <series.hpp>

namespace amt {

template <typename... Ts> struct frame {

    using base_type = std::vector<series<Ts...>>;
    using value_type = typename base_type::value_type;
    using size_type = typename base_type::size_type;
    using reference = typename base_type::reference;
    using const_reference = typename base_type::const_reference;
    using pointer = typename base_type::pointer;
    using const_pointer = typename base_type::const_pointer;
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using reverse_iterator = typename base_type::reverse_iterator;
    using const_reverse_iterator = typename base_type::const_reverse_iterator;
    using col_base_type =
        boost::bimaps::bimap<boost::bimaps::unordered_set_of<std::string>,
                             boost::bimaps::unordered_set_of<size_type>>;
    using col_value_type = typename col_base_type::value_type;

    constexpr frame() noexcept = default;
    constexpr frame(frame const &other)
        : m_data(other.m_data), m_name(other.m_name) {}

    constexpr frame(frame &&other)
        : m_data(std::move(other.m_data)), m_name(std::move(other.m_name)) {}

    constexpr frame &operator=(frame const &other) = default;
    constexpr frame &operator=(frame &&other) noexcept = default;

    ~frame() = default;

    constexpr frame(size_type row, size_type col)
        : m_data(col, value_type(row)) {
        generate_name(0ul);
    }

    constexpr frame(size_type sz) : m_data(sz) { generate_name(0ul); }

    constexpr frame(size_type sz, value_type val) : m_data(sz, std::move(val)) {
        generate_name(0ul);
    }

    frame(std::vector<std::string> v) : m_data(v.size()) {
        size_type i{};
        for (; i < v.size(); ++i) {
            m_name.insert(col_value_type(std::move(v[i]), i));
        }
    }

    frame(std::initializer_list<std::string> li)
        : frame(std::move(std::vector<std::string>(std::move(li)))) {}

    frame(std::vector<value_type> v) : m_data(std::move(v)) {
        generate_name(0ul);
    }

    frame(std::initializer_list<value_type> li)
        : frame(std::move(std::vector<value_type>(std::move(li)))) {}

    frame(std::vector<std::pair<std::string_view, value_type>> v)
        : m_data(v.size()) {
        size_type i{};
        for (; i < cols(); ++i) {
            std::string name(std::move(v[i].first));
            m_data[i] = std::move(v[i].second);
            m_name.insert(col_value_type(std::move(name), i));
        }
    }

    frame(std::initializer_list<std::pair<std::string_view, value_type>> li)
        : frame(std::move(std::vector<std::pair<std::string_view, value_type>>(
              std::move(li)))) {}

    frame(FrameView auto &&fv) : frame(fv.rows(), fv.cols()) {
        for (auto i = 0ul; i < fv.cols(); ++i) {
            for (auto j = 0ul; j < fv.rows(); ++j) {
                m_data[i][j] = fv[i][j];
            }
        }

        set_name(std::move(fv.name_to_vector()));
    }

    constexpr reference operator[](size_type k) noexcept { return m_data[k]; }

    constexpr const_reference operator[](size_type k) const noexcept {
        return m_data[k];
    }

    constexpr reference operator[](std::string_view k) {
        return m_data[index(k)];
    }

    constexpr const_reference operator[](std::string_view k) const {
        return m_data[index(k)];
    }

    constexpr reference at(size_type k) { return m_data.at(k); }

    constexpr const_reference at(size_type k) const { return m_data.at(k); }

    constexpr reference at(std::string_view k) { return m_data.at(index(k)); }

    constexpr const_reference at(std::string_view k) const {
        return m_data.at(index(k));
    }

    constexpr auto &at(size_type r, size_type c) { return m_data.at(c).at(r); }

    constexpr auto const &at(size_type r, size_type c) const {
        return m_data.at(c).at(r);
    }

    constexpr auto &at(std::string_view k, size_type r) {
        return m_data.at(index(k)).at(r);
    }

    constexpr auto const &at(std::string_view k, size_type r) const {
        return m_data.at(index(k)).at(r);
    }

    [[nodiscard]] inline constexpr size_type cols() const noexcept {
        return m_data.size();
    }

    [[nodiscard]] inline constexpr size_type rows() const noexcept {
        return empty() ? 0ul : m_data[0].size();
    }

    [[nodiscard]] inline constexpr bool empty() const noexcept {
        return m_data.empty();
    }

    [[nodiscard]] inline std::string name(size_type k) const {
        auto const &temp = index_to_name_map();
        if (auto it = temp.find(k); it != temp.end()) {
            return it->second;
        } else {
            return "";
        }
    }

    [[nodiscard]] inline std::vector<std::string> name_to_vector() const {
        std::vector<std::string> temp(cols());
        for (auto const &[n, i] : index_to_name_map()) {
            temp[i] = n;
        }
        return temp;
    }

    inline void set_name(std::vector<std::string> v) {
        if (v.size() != cols()) {
            throw std::runtime_error(
                "amt::frame::set_name(std::vector<std::string>) : "
                "size mismatch");
        }
        m_name.clear();
        for (auto i = 0ul; i < v.size(); ++i) {
            m_name.insert(col_value_type(std::move(v[i]), i));
        }
    }

    inline void set_name(frame const &f) {
        if (f.cols() != cols()) {
            throw std::runtime_error("amt::frame::set_name(frame const&) : "
                                     "size mismatch");
        }
        set_name(std::move(f.name_to_vector()));
    }

    template <bool B> inline void set_name(view<frame, B> const &f) {
        if (f.cols() != cols()) {
            throw std::runtime_error(
                "amt::frame::set_name(std::vector<std::string>) : "
                "size mismatch");
        }
        set_name(std::move(f.name_to_vector()));
    }

    [[nodiscard]] inline size_type index(std::string_view s) const {
        return name_to_index_map().at(s.data());
    }

    inline bool replace(size_type k, std::string_view nname) {
        auto &temp = m_name.right;

        if (auto it = temp.find(k); it != temp.end()) {
            temp.replace_data(it, std::string(std::move(nname)));
            return true;
        }
        return false;
    }

    inline bool replace(std::string_view name, size_type nidx) {
        auto &temp = m_name.left;
        if (auto it = temp.find(name.data()); it != temp.end()) {
            temp.replace_data(it, nidx);
            return true;
        }
        return false;
    }

    inline void reset_name() {
        m_name.clear();
        generate_name(0ul);
    }

    [[nodiscard]] inline constexpr auto const &
    index_to_name_map() const noexcept {
        return m_name.right;
    }

    [[nodiscard]] inline constexpr auto const &
    name_to_index_map() const noexcept {
        return m_name.left;
    }

    [[nodiscard]] inline constexpr reference back() { return m_data.back(); }

    [[nodiscard]] inline constexpr const_reference back() const {
        return m_data.back();
    }

    [[nodiscard]] inline constexpr pointer data() noexcept {
        return m_data.data();
    }

    [[nodiscard]] inline constexpr const_pointer data() const noexcept {
        return m_data.data();
    }

    [[nodiscard]] inline constexpr auto &name_base() noexcept { return m_name; }

    [[nodiscard]] inline constexpr auto const &name_base() const noexcept {
        return m_name;
    }

    template <typename... Args>
    [[nodiscard]] inline constexpr decltype(auto) operator()(Args &&... args) {
        return at(std::forward<Args>(args)...);
    }

    template <typename... Args>
    [[nodiscard]] inline constexpr decltype(auto)
    operator()(Args &&... args) const {
        return at(std::forward<Args>(args)...);
    }

    template <typename T>
    [[nodiscard]] inline constexpr view<frame, false>
    operator()(basic_slice<T> fslice, basic_slice<T> sslice = {}) {
        return {*this, std::move(fslice), std::move(sslice)};
    }

    template <typename T>
    [[nodiscard]] inline constexpr view<frame, true>
    operator()(basic_slice<T> fslice, basic_slice<T> sslice = {}) const {
        return {*this, std::move(fslice), std::move(sslice)};
    }

    template <typename T>
    [[nodiscard]] inline constexpr view<frame, false> operator()() {
        return {*this};
    }

    template <typename T>
    [[nodiscard]] inline constexpr view<frame, true> operator()() const {
        return {*this};
    }

    [[nodiscard]] inline constexpr iterator begin() noexcept {
        return m_data.begin();
    }

    [[nodiscard]] inline constexpr iterator end() noexcept {
        return m_data.end();
    }

    [[nodiscard]] inline constexpr const_iterator begin() const noexcept {
        return m_data.begin();
    }

    [[nodiscard]] inline constexpr const_iterator end() const noexcept {
        return m_data.end();
    }

    [[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept {
        return m_data.rbegin();
    }

    [[nodiscard]] inline constexpr reverse_iterator rend() noexcept {
        return m_data.rend();
    }

    [[nodiscard]] inline constexpr const_reverse_iterator
    rbegin() const noexcept {
        return m_data.rbegin();
    }

    [[nodiscard]] inline constexpr const_reverse_iterator
    rend() const noexcept {
        return m_data.rend();
    }

    [[nodiscard]] inline iterator erase(iterator pos) {
        auto idx = std::distance(m_data.begin(), pos);
        auto &temp = m_name.right;
        if (auto it = temp.find(idx); it != temp.end()) {
            temp.erase(it);
        }
        return m_data.erase(pos);
    }

    [[nodiscard]] inline iterator erase(const_iterator pos) {
        auto idx = std::distance(m_data.begin(), pos);
        auto &temp = m_name.right;
        if (auto it = temp.find(idx); it != temp.end()) {
            temp.erase(it);
        }
        return m_data.erase(pos);
    }

    [[nodiscard]] inline iterator erase(size_type pos) {
        auto &temp = m_name.right;
        if (auto it = temp.find(pos); it != temp.end()) {
            temp.erase(it);
        }
        return m_data.erase(m_data.begin() + pos);
    }

    inline void erase_name(size_type pos) {
        auto &temp = m_name.right;
        if (auto it = temp.find(pos); it != temp.end()) {
            temp.erase(it);
        }
    }

    inline void erase_name(std::string_view k) {
        auto &temp = m_name.left;
        if (auto it = temp.find(k.data()); it != temp.end()) {
            temp.erase(it);
        }
    }

    inline void erase_row(size_type k) {
        for (auto i = 0ul; i < cols(); ++i) {
            m_data[i].erase(static_cast<typename iterator::difference_type>(k));
        }
    }

    [[nodiscard]] inline iterator erase(iterator first, iterator last) {
        for (auto i = std::distance(m_data.begin(), first);
             i < std::distance(first, last); ++i) {
            auto &temp = m_name.right;
            if (auto it = temp.find(i); it != temp.end()) {
                temp.erase(it);
            }
        }
        return m_data.erase(first, last);
    }

    [[nodiscard]] inline iterator erase(const_iterator first,
                                        const_iterator last) {
        for (auto i = std::distance(m_data.begin(), first);
             i < std::distance(first, last); ++i) {
            auto &temp = m_name.right;
            if (auto it = temp.find(i); it != temp.end()) {
                temp.erase(it);
            }
        }
        return m_data.erase(first, last);
    }

    inline void clear() {
        m_data.clear();
        m_name.clear();
    }

    inline void resize(size_type sz) {
        m_data.resize(sz);
        auto prev_size = m_name.size();
        for (auto i = sz; i < prev_size; ++i)
            erase_name(i);
        generate_name(prev_size);
    }

    inline void resize(size_type rows, size_type cols) {
        m_data.resize(cols, value_type(rows));
        auto prev_size = m_name.size();
        for (auto i = cols; i < prev_size; ++i)
            erase_name(i);
        generate_name(prev_size);
    }

    inline constexpr void reserve(size_type cap) {
        m_data.reserve(cap);
        m_name.reserve(cap);
    }

    inline void resize(size_type sz, value_type val) {
        m_data.resize(sz, std::move(val));
        auto prev_size = m_name.size();
        for (auto i = sz; i < prev_size; ++i)
            erase_name(i);
        generate_name(prev_size);
    }

    friend void swap(frame &LHS, frame &RHS) {
        std::swap(LHS.m_data, RHS.m_data);
        std::swap(LHS.m_name, RHS.m_name);
    }

  private:
    inline constexpr void generate_name(size_type i) {
        auto &right_map = m_name.right;
        if (i >= cols())
            return;
        while (right_map.count(i)) {
            ++i;
        };
        for (; i < cols(); ++i) {
            m_name.insert(col_value_type(std::move(std::to_string(i)), i));
        }
    };

  private:
    base_type m_data;
    col_base_type m_name;
};

} // namespace amt

#endif // AMT_FRAME_HPP
