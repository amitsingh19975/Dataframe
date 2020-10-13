#if !defined(AMT_CORE_ERROR_HPP)
#define AMT_CORE_ERROR_HPP

namespace amt {

    enum class error_code : unsigned char{
        #define AMT_TYPE_SEPARATOR ,
        #define AMT_ERROR_CODE(NAME,MESS) NAME
        #include <core/error_codes.def>
    };

    constexpr char const* to_string(error_code e) noexcept{
        switch(e){
            #define AMT_ERROR_CODE(NAME,MESS) case error_code::NAME : return MESS ;
            #include <core/error_codes.def>
            default: return "Unknown error";
        }

        return "Unreachable code";
    }

} // namespace amt

#endif // AMT_CORE_ERROR_HPP
