#if !defined(AMT_FRAME_HPP)
#define AMT_FRAME_HPP

#include <frame_view.hpp>
#include <series.hpp>

namespace amt {

template <typename... Ts> struct frame {

    using base_type = std::vector<series<Ts...>>;
    using value_type = typename base_type::value_type;
    using box_type = typename value_type::value_type;
    using size_type = typename base_type::size_type;
    using reference = typename base_type::reference;
    using const_reference = typename base_type::const_reference;
    using pointer = typename base_type::pointer;
    using const_pointer = typename base_type::const_pointer;
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using reverse_iterator = typename base_type::reverse_iterator;
    using const_reverse_iterator = typename base_type::const_reverse_iterator;

    constexpr frame() noexcept = default;
    constexpr frame(frame const &other) : m_data(other.m_data) {}

    constexpr frame(frame &&other) : m_data(std::move(other.m_data)) {}

    constexpr frame &operator=(frame const &other) = default;
    constexpr frame &operator=(frame &&other) noexcept = default;

    ~frame() = default;

    constexpr frame(size_type col, size_type row)
        : m_data(col, value_type(row)) {
        generate_name(0ul);
    }

    constexpr frame(size_type sz) : m_data(sz) { generate_name(0ul); }

    constexpr frame(size_type sz, size_type row, box_type val) : m_data(sz, value_type(row,std::move(val))) {
        generate_name(0ul);
    }

    constexpr frame(size_type sz, value_type val) : m_data(sz, std::move(val)) {
        generate_name(0ul);
    }

    frame(std::vector<value_type> v) : m_data(std::move(v)) {
        generate_name(0ul);
        for (auto i = 0ul; i < cols(); ++i) {
            if (m_data[i].size() != rows()) {
                throw std::runtime_error(
                    "amt::frame(std::vector<std::pair<std::string_view, "
                    "value_type>>) : "
                    "all the cols should have same number of rows");
            }
        }
    }

    frame(std::initializer_list<value_type> li)
        : frame(std::move(std::vector<value_type>(std::move(li)))) {}

    frame(FrameView auto &&fv) : frame(fv.rows(), fv.cols()) {

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < fv.cols(); ++i) {
            for (auto j = 0ul; j < fv.rows(); ++j) {
                m_data[i][j] = fv[i][j];
            }
        }
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

    constexpr auto &at(size_type c, size_type r) { return m_data.at(c).at(r); }

    constexpr auto const &at(size_type c, size_type r) const {
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

    [[nodiscard]] inline std::string const &name(size_type k) const {
        if (k >= cols()) {
            throw std::out_of_range("amt::frame::name(size_type)");
        }
        return m_data[k].name();
    }

    [[nodiscard]] inline std::string &name(size_type k) {
        if (k >= cols()) {
            throw std::out_of_range("amt::frame::name(size_type)");
        }
        return m_data[k].name();
    }

    [[nodiscard]] inline std::vector<std::string> names_to_vector() const {
        std::vector<std::string> temp(cols());
        auto i = 0ul;
        for (auto const &el : m_data) {
            temp[i++] = el.name();
        }
        return temp;
    }

    inline void set_name(std::vector<std::string> v) {
        if (v.size() != cols()) {
            throw std::runtime_error(
                "amt::frame::set_name(std::vector<std::string>) : "
                "size mismatch");
        }
        for (auto i = 0ul; i < v.size(); ++i) {
            m_data[i].name() = std::move(v[i]);
        }
    }

    [[nodiscard]] inline size_type index(std::string_view s) const {
        auto i = 0ul;
        for (auto const &el : *this) {
            if (el.name() == s)
                return i;
            ++i;
        }
        throw std::runtime_error("amt::frame::index(std::string_view): "
                                 "name not found");
    }

    inline void replace(size_type k, std::string_view name) {
        m_data[k].name() = std::move(name);
    }

    inline void reset_name() {
        std::for_each(m_data.begin(), m_data.end(),
                      [](auto &el) { el.name().clear(); });
        generate_name(0ul);
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

    void push_back(frame f) {
        for (auto i = 0ul; i < f.cols(); ++i) {
            m_data.push_back(std::move(f[i]));
        }
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

    inline void reset_names() noexcept {
        for (auto i = 0ul; i < m_data.size(); ++i)
            m_data[i] = std::to_string(i);
    }

    inline iterator erase(iterator pos) { return m_data.erase(pos); }

    inline iterator erase(const_iterator pos) { return m_data.erase(pos); }

    inline iterator erase(size_type pos) {
        return erase(m_data.begin() +
                     static_cast<typename iterator::difference_type>(pos));
    }

    inline void erase_row(size_type k) {
        for (auto i = 0ul; i < cols(); ++i) {
            m_data[i].erase(static_cast<typename iterator::difference_type>(k));
        }
    }

    [[nodiscard]] inline iterator erase(iterator first, iterator last) {
        return m_data.erase(first, last);
    }

    [[nodiscard]] inline iterator erase(const_iterator first,
                                        const_iterator last) {
        return m_data.erase(first, last);
    }

    inline void clear() { m_data.clear(); }

    inline void resize(size_type sz) {
        auto prev_size = m_data.size();
        m_data.resize(sz);
        generate_name(prev_size);
    }

    inline void resize(size_type cols, size_type rows) {
        auto prev_size = m_data.size();
        m_data.resize(cols, value_type(rows));
        generate_name(prev_size);
    }

    inline constexpr void reserve(size_type cap) { m_data.reserve(cap); }

    inline void resize(size_type sz, value_type val) {
        auto prev_size = m_data.size();
        m_data.resize(sz, std::move(val));
        generate_name(prev_size);
    }

    friend void swap(frame &LHS, frame &RHS) {
        std::swap(LHS.m_data, RHS.m_data);
    }

  private:
    constexpr void generate_name(size_type i) {
        for (auto j = i; j < m_data.size(); ++j) {
            auto &n = m_data[j].name();
            if (n.empty())
                n = std::to_string(j);
        }
    };

  private:
    base_type m_data;
};

} // namespace amt

#endif // AMT_FRAME_HPP
