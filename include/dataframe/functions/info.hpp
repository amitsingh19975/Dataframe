#if !defined(AMT_DATAFRAME_FUNCTIONS_INFO_HPP)
#define AMT_DATAFRAME_FUNCTIONS_INFO_HPP

#include <cmath>
#include <dataframe/functions/box_maths.hpp>
#include <dataframe/functions/count.hpp>
#include <dataframe/functions/pretty_string.hpp>
#include <dataframe/series_utils.hpp>

namespace amt {

namespace fn {

struct info_t {

    template <Series SeriesType>
    std::string operator()(SeriesType const &in) const {
        using box_type = typename SeriesType::value_type;
        basic_frame<box_type> temp(3u, 1u);

        temp[0].name("Column");
        temp[1].name("Non-Null Count");
        temp[2].name("DType");

        auto sz = count_null(in);
        temp[0][0] = in.name();
        temp[1][0] = in.size() - sz;
        temp[2][0] = type_to_string(in);

        return pretty_string(temp);
    }

    template <Frame FrameType>
    std::string operator()(FrameType const &in) const {
        frame_result_t<FrameType> temp(3u, in.cols());

        temp[0].name("Column");
        temp[1].name("Non-Null Count");
        temp[2].name("DType");

        auto i = 0u;
        for (auto const &el : in) {
            auto sz = count_null(el);
            temp[0][i] = el.name();
            temp[1][i] = el.size() - sz;
            temp[2][i] = type_to_string(el);
            ++i;
        }

        return pretty_string(temp);
    }

  private:
    constexpr std::size_t count_null(auto const &in) const {
        return count(in, [](auto const &val) {
            return visit(val, []<typename T>(T const &el) {
                if constexpr (std::numeric_limits<T>::has_quiet_NaN) {
                    if (std::isnan(el))
                        return true;
                } else if constexpr (is_monostate_v<T>) {
                    return true;
                } else if constexpr (std::is_pointer_v<T>) {
                    if (el == nullptr)
                        return true;
                }
                return false;
            });
        });
    }
};

} // namespace fn

inline static constexpr auto info = fn::info_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_INFO_HPP
