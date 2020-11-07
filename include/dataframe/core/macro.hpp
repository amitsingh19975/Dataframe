#if !defined(AMT_DATAFRAME_CORE_MACRO_HPP)
#define AMT_DATAFRAME_CORE_MACRO_HPP

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifdef __LINE__
#define ERR_CSTR(MESS) "( Line: " STR(__LINE__) " ) " MESS 
#else
#define ERR_CSTR(MESS) MESS
#endif

#endif // AMT_DATAFRAME_CORE_MACRO_HPP
