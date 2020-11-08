#if !defined(AMT_DATAFRAME_FUNCTIONS_INFER_HPP)
#define AMT_DATAFRAME_FUNCTIONS_INFER_HPP

#include <dataframe/functions/cast.hpp>
#include <dataframe/traits/basic_traits.hpp>

namespace amt {

namespace fn {

template <bool ApplyDefaultNaList> struct infer_t {

    template <Box BoxType>
    constexpr BoxType operator()(BoxType const &b,
                                 name_list const &na_list = {}) const {
        if constexpr (ApplyDefaultNaList) {
            if (is<std::string>(b) &&
                (in(na_list, get<std::string>(b)) ||
                 in(default_nan_list, get<std::string>(b)))) {
                return BoxType();
            }
        } else {
            if (is<std::string>(b) && in(na_list, get<std::string>(b))) {
                return BoxType();
            }
        }

        if (box_infer<long long>(b)) {
            return cast<long long, ApplyDefaultNaList>(b);
        } else if (box_infer<double>(b)) {
            return cast<double, ApplyDefaultNaList>(b);
        } else {
            return cast<std::string, ApplyDefaultNaList>(b);
        }
    }

    template <Series SeriesType, Series SeriesOut>
    constexpr SeriesOut &operator()(SeriesType const &in, SeriesOut &out,
                                    name_list const &na_list = {}) const {
        std::size_t count[3] = {0};
        auto sz = in.size();
        for (auto i = 0ul; i < sz; ++i) {
            auto &b = in[i];
            if constexpr (ApplyDefaultNaList) {
                if (count_if_in_list(count, b, default_nan_list) ||
                    count_if_in_list(count, b, na_list)) {
                    continue;
                }
            } else {
                if (count_if_in_list(count, b, na_list)) {
                    continue;
                }
            }
            if (box_infer<long long>(b)) {
                ++count[INT64];
                ++count[DOUBLE];
            } else if (box_infer<double>(b)) {
                ++count[DOUBLE];
            } else {
                ++count[STRING];
            }
        }

        if (sz == count[INT64]) {
            cast<long long, ApplyDefaultNaList>(in, out);
        } else if (count[DOUBLE] > count[STRING]) {
            cast<double, ApplyDefaultNaList>(in, out);
        } else {
            cast<std::string, ApplyDefaultNaList>(in, out);
        }
        return out;
    }

    template <Series SeriesType>
    constexpr series_result_t<SeriesType>
    operator()(SeriesType const &in, name_list const &na_list = {}) const {
        series_result_t<SeriesType> out(in.name(), in.size());
        this->operator()(in, out, na_list);
        return out;
    }

    template <Series SeriesType>
    constexpr series_result_t<SeriesType>
    operator()(SeriesType &in, tag::inplace_t,
               name_list const &na_list = {}) const {
        auto out = this->operator()(in, na_list);
        in = std::move(out);
        return in;
    }

    template <Frame FrameType, Frame FrameOut>
    constexpr FrameOut &operator()(FrameType const &in, FrameOut &out,
                                   name_list const &na_list = {}) const {
        for (auto i = 0u; i < in.cols(); ++i) {
            this->operator()(in[i], out[i], na_list);
        }
        return out;
    }

    template <Frame FrameType>
    constexpr frame_result_t<FrameType>
    operator()(FrameType const &in, name_list const &na_list = {}) const {
        frame_result_t<FrameType> out(in.shape());
        this->operator()(in, out, na_list);
        return out;
    }

    template <Frame FrameType>
    constexpr frame_result_t<FrameType>
    operator()(FrameType &in, tag::inplace_t,
               name_list const &na_list = {}) const {
        auto out = this->operator()(in, na_list);
        in = std::move(out);
        return in;
    }

  private:
    enum : unsigned char { INT64 = 0u, DOUBLE = 1u, STRING = 2u };

    template <typename To, typename U>
    bool can_be_casted_helper(U &&val) const {
        if constexpr (conv::HasCast<To, U>) {
            To temp;
            return conv::cast(temp, std::forward<U>(val));
        }
        return false;
    }

    template <typename To> constexpr bool box_infer(Box auto const &b) const {
        return visit(b, [this](auto const &u) {
            return this->can_be_casted_helper<To>(u);
        });
    }

    constexpr bool in(auto &&list, std::string_view name) const noexcept {
        auto it = std::find(list.begin(), list.end(), name);
        return it != list.end();
    }

    constexpr bool count_if_in_list(std::size_t *count, Box auto &&b,
                                    auto &&list) const {
        if (is<std::string>(b)) {
            if (in(list, get<std::string>(b))) {
                ++count[INT64];
                ++count[DOUBLE];
                ++count[STRING];
                return true;
            }
        }
        return false;
    }
};

} // namespace fn

template <bool ApplyDefaultNaList = true>
inline static constexpr auto infer = fn::infer_t<ApplyDefaultNaList>{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_INFER_HPP
