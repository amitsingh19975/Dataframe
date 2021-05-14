#if !defined( AMT_DATAFRAME_CORE_TAGS_HPP )
#define AMT_DATAFRAME_CORE_TAGS_HPP

namespace amt::tag {

    struct dynamic_storage {};
    struct static_storage {};

    struct dummy {};

    template<typename T>
    struct dtype{};

} // namespace amt::tag

#endif // AMT_DATAFRAME_CORE_TAGS_HPP
