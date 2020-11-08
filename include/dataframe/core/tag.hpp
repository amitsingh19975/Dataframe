#if !defined(AMT_DATAFRAME_CORE_TAG_HPP)
#define AMT_DATAFRAME_CORE_TAG_HPP

namespace amt::tag {

struct inplace_t {};

inline static constexpr inplace_t inplace{};

struct sample_t {};

inline static constexpr sample_t sample{};

struct right_to_left_t {};

inline static constexpr right_to_left_t right_to_left{};

struct col_t {};
struct row_t {};

inline static constexpr col_t col{};
inline static constexpr row_t row{};

struct nan_t {};
inline static constexpr nan_t nan{};

} // namespace amt::tag

#endif // AMT_DATAFRAME_CORE_TAG_HPP
