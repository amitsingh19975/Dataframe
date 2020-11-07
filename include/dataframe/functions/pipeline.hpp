#if !defined(AMT_DATAFRAME_FUNCTIONS_PIPELINE_HPP)
#define AMT_DATAFRAME_FUNCTIONS_PIPELINE_HPP

#include <dataframe/core/pipeline_utils.hpp>

namespace amt {

template <typename... Fns> struct pipeline {

    using base_type = std::tuple<Fns...>;

    constexpr pipeline(pipeline const &other) noexcept = default;
    constexpr pipeline(pipeline &&other) noexcept = default;
    constexpr pipeline &operator=(pipeline const &other) noexcept = default;
    constexpr pipeline &operator=(pipeline &&other) noexcept = default;
    constexpr ~pipeline() noexcept = default;

    constexpr pipeline(Fns &&... fns) noexcept
        : m_fns(std::forward<Fns>(fns)...) {}

    template <typename TupleType>
    requires is_tuple_v<TupleType> constexpr decltype(auto)
    operator()(TupleType &&args, tag::right_to_left_t) const {
        return invoke_pipeline(m_fns, std::forward<TupleType>(args),
                               tag::right_to_left_t{});
    }

    template <typename TupleType>
    requires is_tuple_v<TupleType> constexpr decltype(auto)
    operator()(TupleType &&args) const {
        return invoke_pipeline(m_fns, std::forward<TupleType>(args));
    }

  private:
    base_type m_fns;
};

template <typename... Fns> pipeline(Fns &&...) -> pipeline<Fns...>;

template <Pipeline... Ps> struct connect_pipelines {

    using base_type = std::tuple<Ps...>;

    constexpr connect_pipelines(connect_pipelines const &other) noexcept =
        default;
    constexpr connect_pipelines(connect_pipelines &&other) noexcept = default;
    constexpr connect_pipelines &
    operator=(connect_pipelines const &other) noexcept = default;
    constexpr connect_pipelines &
    operator=(connect_pipelines &&other) noexcept = default;
    constexpr ~connect_pipelines() noexcept = default;

    constexpr connect_pipelines(Ps &&... ps) noexcept
        : m_pipes(std::forward<Ps>(ps)...) {}

    template <typename TupleType>
    requires is_tuple_v<TupleType> constexpr decltype(auto)
    operator()(TupleType &&args, tag::right_to_left_t) const {
        return invoke_pipeline(m_pipes, std::forward<TupleType>(args),
                               tag::right_to_left_t{});
    }

    template <typename TupleType>
    requires is_tuple_v<TupleType> constexpr decltype(auto)
    operator()(TupleType &&args) const {
        return invoke_pipeline(m_pipes, std::forward<TupleType>(args));
    }

  private:
    base_type m_pipes;
};

template <typename... Fns>
connect_pipelines(Fns &&...) -> connect_pipelines<Fns...>;

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_PIPELINE_HPP
