#if !defined(AMT_FRAME_VIEW_HPP)
#define AMT_FRAME_VIEW_HPP

#include <boost/bimap/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <core/concepts.hpp>
#include <slice.hpp>

namespace amt {

template <typename T, bool Const> struct view;

} // namespace amt

namespace amt {

template <bool Const, typename... Ts> struct view<frame<Ts...>, Const> {
    static constexpr auto const is_const = Const;

    using view_of = frame<Ts...>;
    using size_type = typename view_of::size_type;

    using slice_type = basic_slice<size_type>;

    using base_type = std::vector<view<series<Ts...>, Const>>;
    using value_type = typename base_type::value_type;
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using reverse_iterator = typename base_type::reverse_iterator;
    using const_reverse_iterator = typename base_type::const_reverse_iterator;
    using pointer = typename base_type::pointer;
    using const_pointer = typename base_type::const_pointer;
    using reference = typename iterator::reference;
    using const_reference = typename base_type::const_reference;

    constexpr view() noexcept = default;

    constexpr view(view const &other) = default;

    constexpr view(view &&other) noexcept = default;

    constexpr view &operator=(view const &other) = default;

    constexpr view &operator=(view &&other) noexcept = default;

    ~view() = default;

    constexpr view(Frame auto &&f, slice_type fsl, slice_type ssl)
        : m_fslice(detail::norm_slice(std::move(fsl), f.cols())) {
        if (f.cols() != 0) {
            auto tssl = detail::norm_slice(std::move(ssl), f.rows());

#pragma omp parallel for schedule(static)
            for (auto i = m_fslice.first(); i <= m_fslice.last();
                 i += m_fslice.step()) {
                m_data.push_back(f[i](tssl));
            }
        }
    }

    constexpr view(Frame auto &&s) : view(s, slice_type{}, slice_type{}) {}

    [[nodiscard]] inline constexpr size_type cols() const noexcept {
        return m_data.size();
    }

    [[nodiscard]] inline constexpr size_type rows() const noexcept {
        return empty() ? 0ul : m_data[0].size();
    }

    [[nodiscard]] inline constexpr bool empty() const noexcept {
        return m_data.empty();
    }

    [[nodiscard]] inline std::string_view name(size_type k) const {
        if (k >= cols()) {
            throw std::out_of_range("amt::view<series>::name(size_type)");
        }
        return m_data[k].name();
    }

    [[nodiscard]] inline std::vector<std::string> names_to_vector() const {
        std::vector<std::string> temp(cols());

#pragma omp parallel for schedule(static)
        for (auto i = 0ul; i < cols(); ++i) {
            temp[i] = name(i);
        }
        return temp;
    }

    [[nodiscard]] inline constexpr reference operator[](size_type k) noexcept {
        return m_data[k];
    }

    [[nodiscard]] inline constexpr const_reference
    operator[](size_type k) const noexcept {
        return m_data[k];
    }

    [[nodiscard]] inline constexpr reference at(size_type k) {
        return m_data.at(k);
    }

    [[nodiscard]] inline constexpr const_reference at(size_type k) const {
        return m_data.at(k);
    }

    constexpr reference operator[](std::string_view k) {
        return m_data[index(k)];
    }

    constexpr const_reference operator[](std::string_view k) const {
        return m_data[index(k)];
    }

    constexpr reference at(std::string_view k) { return m_data[index(k)]; }

    constexpr const_reference at(std::string_view k) const {
        return m_data[index(k)];
    }

    constexpr auto &at(size_type c, size_type r) { return at(c).at(r); }

    constexpr auto const &at(size_type c, size_type r) const {
        return at(c).at(r);
    }

    constexpr auto &at(std::string_view k, size_type r) {
        return m_data[index(k)].at(r);
    }

    constexpr auto const &at(std::string_view k, size_type r) const {
        return m_data[index(k)].at(r);
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

    [[nodiscard]] inline size_type index(std::string_view s) const {
        auto idx = 0ul;
        for (; idx < cols(); ++idx) {
            if (m_data[idx].name() == s)
                break;
        }
        return idx;
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

    [[nodiscard]] inline constexpr slice_type slice() const noexcept {
        return m_fslice;
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

    friend void swap(view &LHS, view &RHS) noexcept {
        std::swap(LHS.m_data, RHS.m_data);
        std::swap(LHS.m_slice, RHS.m_slice);
        std::swap(LHS.m_size, RHS.m_size);
    }

  private:
    base_type m_data;
    slice_type m_fslice;
};

} // namespace amt

#endif // AMT_FRAME_VIEW_HPP
