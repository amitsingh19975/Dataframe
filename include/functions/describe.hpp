#if !defined(AMT_DATAFRAME_FUNCTIONS_DESCRIBE_HPP)
#define AMT_DATAFRAME_FUNCTIONS_DESCRIBE_HPP

#include <functions/pretty_string.hpp>
#include <functions/count.hpp>
#include <functions/frequency.hpp>
#include <functions/box_maths.hpp>
#include <series_utils.hpp>
#include <functions/stats.hpp>
#include <functions/cast.hpp>

namespace amt {

namespace fn{
    
    struct describe_t{

        template<Series SeriesType>
        std::string operator()(SeriesType const& in, bool all = false) const {
            using series_type = series_result_t<SeriesType>;
            using box_type = typename series_type::value_type;
            
            basic_frame<box_type> temp;
            if(!all){
                metrics_init(temp, in, 2ul);
                fill(in, temp[1]);
            }else{
                all_metrics_init(temp,2ul);
                this->all(in,temp[1]);
            }
            return pretty_string(temp, 4u, false);
        }

        template<Frame FrameType>
        std::string operator()(FrameType const& in) const {
            auto cols = in.cols() + 1ul;
            frame_result_t<FrameType> temp;

            all_metrics_init(temp,cols);
            auto i = 1ul;
            for(auto const& el : in){
                this->all(el,temp[i++]);
            }

            return pretty_string(temp,4u, false);
        }

    private:
        constexpr std::size_t count_null(auto const& in) const{
            return count(in, [](auto const& val){
                return visit(val, []<typename T>(T const& el){
                    if constexpr(std::numeric_limits<T>::has_quiet_NaN){
                        if( std::isnan(el) ) return true;
                    }else if constexpr(is_monostate_v<T>){
                        return true;
                    }else if constexpr(std::is_pointer_v<T>){
                        if(el == nullptr) return true;
                    }
                    return false;
                });
            });
        }

        constexpr void fill(auto const& in, auto& out) const{
            if(is_floating_point(in)){
                numeric_no_casting(in, out);
            }else if( is_integer(in) || is_bool(in) ){
                numeric_casting(in, out);
            }else{
                categorical(in, out);
            }
        }

        constexpr void numeric_metrics_init(PureFrame auto& out, std::size_t cols) const{
            out.resize(cols, 6ul);
            out[0].name("");
            out[0][0] = "mean";
            out[0][1] = "std";
            out[0][2] = "count";
            out[0][3] = "min";
            out[0][4] = "max";
            out[0][5] = "dtype";
        }

        constexpr void categorical_metrics_init(PureFrame auto& out, std::size_t cols) const{
            out.resize(cols, 5ul);
            out[0].name("");
            out[0][0] = "count";
            out[0][1] = "unique";
            out[0][2] = "top";
            out[0][3] = "freq";
            out[0][4] = "dtype";
        }

        constexpr void metrics_init(PureFrame auto& out, Series auto const& in, std::size_t cols) const{
            if(is_floating_point(in)){
                numeric_metrics_init(out, cols);
            }else if( is_integer(in) || is_bool(in) ){
                numeric_metrics_init(out, cols);
            }else{
                categorical_metrics_init(out,cols);
            }
        }

        constexpr void all_metrics_init(PureFrame auto& out, std::size_t cols) const{
            out.resize(cols, 9ul);
            out[0].name("");
            out[0][0] = "mean";
            out[0][1] = "std";
            out[0][2] = "min";
            out[0][3] = "max";
            out[0][4] = "count";
            out[0][5] = "unique";
            out[0][6] = "top";
            out[0][7] = "freq";
            out[0][8] = "dtype";
        }

        constexpr void numeric_no_casting(auto const& in, auto& out) const{
            out.name(in.name());
            out.dtype(dtype<double>());
            auto m = mean<double>(in);
            auto s = sdev<double>(in);
            auto c = static_cast<double>(in.size() - count_null(in));
            auto [mi,ma] = minmax<double>(in);
            out[0] = m;
            out[1] = s;
            out[2] = c;
            out[3] = mi;
            out[4] = ma;
            out[5] = type_to_string(in);
        }

        constexpr void numeric_casting(auto const& in, auto& out) const{
            auto temp = cast<double>(in);
            numeric_no_casting(temp, out);
        }

        template<typename T>
        constexpr void categorical(T const& in, auto& out) const{
            out.name(in.name());
            using box_type = typename T::box_type;
            auto c = static_cast<double>(in.size() - count_null(in));
            auto f = freq(in);
            auto mel = box_type{};
            auto ct = 0ul;
            for(auto const& [k,v] : f){
                if(ct < v){
                    ct = v;
                    mel = k;
                }
            }
            out[0] = c;
            out[1] = f.size();
            out[2] = std::move(mel);
            out[3] = ct;
            out[4] = type_to_string(in);
        }

        template<typename T>
        constexpr void all(T const& in, auto& out) const{
            auto tflag = is_floating_point(in) || is_integer(in) || is_bool(in);

            out.name(in.name());

            using box_type = typename T::box_type;
            auto c = static_cast<double>(in.size() - count_null(in));
            if( !tflag ){
                auto f = freq(in);
                auto mel = box_type{};
                auto ct = 0ul;
                for(auto const& [k,v] : f){
                    if(ct < v){
                        ct = v;
                        mel = k;
                    }
                }

                out[5] = f.size();;
                out[6] = std::move(mel);
                out[7] = ct;

            }else{
                double m{};
                double s{};
                double mi{};
                double ma{};
                if( !is_floating_point(in) ){
                    auto temp = cast<double>(in);
                    m = mean<double>(temp);
                    s = sdev<double>(temp);
                    std::tie(mi,ma) = minmax<double>(temp);
                }else{
                    m = mean<double>(in);
                    s = sdev<double>(in);
                    std::tie(mi,ma) = minmax<double>(in);
                }

                out[0] = m;
                out[1] = s;
                out[2] = mi;
                out[3] = ma;
            }
            
            out[4] = c;
            out[8] = type_to_string(in);
        }

    };

} // namespace fn

inline static constexpr auto describe = fn::describe_t{};

} // namespace amt

#endif // AMT_DATAFRAME_FUNCTIONS_DESCRIBE_HPP
